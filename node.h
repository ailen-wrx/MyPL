// node.h and node.cpp are for abstract syntax tree node implementation.

#ifndef _NODE_H
#define _NODE_H

#include <llvm/IR/Value.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "util.h"

using namespace llvm;
using namespace std;

#define BINOP_ASSIGN 260
#define BINOP_PLUS 261
#define BINOP_MINUS 262
#define BINOP_MUL 263
#define BINOP_DIV 264
#define BINOP_MOD 265
#define BINOP_CEQ 266
#define BINOP_CNE 267
#define BINOP_CLT 268
#define BINOP_CLE 269
#define BINOP_CGT 270
#define BINOP_CGE 271
#define BINOP_AND 272
#define BINOP_OR 273

#define TYPE_VAR 1
#define TYPE_INT 2
#define TYPE_DOUBLE 3
#define TYPE_STR 4
#define TYPE_INTARR 5
#define TYPE_BINOP 6
#define TYPE_CALL 7
#define TYPE_ARRIDX 8
#define TYPE_IDENTIFIER 9
#define TYPE_CHAR 10
#define TYPE_DOUBLEARR 11
#define TYPE_STRARR 12

#define STMT_TYPE_EXP 21
#define STMT_TYPE_IF 22
#define STMT_TYPE_WHILE 23
#define STMT_TYPE_FUNDEF 24
#define STMT_TYPE_RET 25

class CodeGenContext;

class Node // general node
{
public:
    virtual Value *codeGen(CodeGenContext &context) = 0;
    virtual string toString() = 0;
};

class NStmt : public Node // statement node
{
public:
    int stmt_type; // statement type (in #define STMT_TYPE_XXX)
    NStmt(int t) : stmt_type(t) {}
};

class NExp : public NStmt // expression node
{
public:
    int type; // expression type (in #define TYPE_XXX)
    NExp(int t) : NStmt(STMT_TYPE_EXP), type(t) {}
    virtual bool isDouble(CodeGenContext &context) { return false; }
};

class NVariable : public NExp // varaible node
{
public:
    string name;
    NVariable(string s) : NExp(TYPE_VAR), name(s) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return name; };
    bool isDouble(CodeGenContext &context) override;
};

class NDouble : public NExp // double node
{
public:
    double value;
    NDouble(double v) : NExp(TYPE_DOUBLE), value(v) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return to_string(value); };
    bool isDouble(CodeGenContext &context) override
    {
        return true;
    }
};

class NInt : public NExp // integer node
{
public:
    int value;
    NInt(int v) : NExp(TYPE_INT), value(v) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return to_string(value); };
    bool isDouble(CodeGenContext &context) override
    {
        return false;
    }
};

class NChar : public NExp
{
public:
    int value;
    NChar(char c) : NExp(TYPE_CHAR), value(int(c)) {}
    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return to_string(char(value)); };
    bool isDouble(CodeGenContext &context) override { return false; }
};

class NStr : public NExp // string node
{
public:
    string value;
    NStr(string v) : NExp(TYPE_STR), value(v.substr(1, v.length() - 2)) {}
    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return value; };
    bool isDouble(CodeGenContext &context) override { return false; }
};

class NArray : public NExp // array node
{
public:
    vector<Value *> elements;
    int size;
    bool isGlobal;
    NArray(int t, int s, bool b) : NExp(t), size(s), isGlobal(b)
    {
        if (t == TYPE_INT)
            type = TYPE_INTARR;
        else if (t == TYPE_DOUBLE)
            type = TYPE_DOUBLE;
        else if (t == TYPE_CHAR)
            type = TYPE_STR;
        else if (t == TYPE_STR)
            type = TYPE_STRARR;
    }

    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return ""; }
    bool isDouble(CodeGenContext &context) override { return type == TYPE_DOUBLEARR; }
};

class NArrayIndex : public NExp // array index node (e.g. a[3])
{
public:
    string arrName;
    NArray *array;
    NExp *index;

    NArrayIndex(string name, NExp *idx) : NExp(TYPE_ARRIDX), arrName(name), array(nullptr), index(idx) {}
    Value *codeGen(CodeGenContext &context) override;
    NArray *getArrayNode(CodeGenContext &context);
    Value *modify(CodeGenContext &context, Value *newVal);
    string toString() override
    {
        return arrName + " [ " + index->toString() + " ] ";
    }
    bool isDouble(CodeGenContext &context) override { return false; }
};

class NBinOp : public NExp // binary operation node
{
public:
    int op;
    NExp *left, *right;
    NBinOp(int c, NExp *l, NExp *r) : NExp(TYPE_BINOP), op(c), left(l), right(r) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        return left->toString() + " " + to_string(op) + " " + right->toString();
    };
    bool isDouble(CodeGenContext &context) override
    {
        return left->isDouble(context) || right->isDouble(context);
    }
};

class NCallFunc : public NExp // calling function node
{
public:
    string funcName;
    vector<NExp *> args;
    map<string, NExp *> vars_reserved;

    NCallFunc(string name, vector<NExp *> &a) : NExp(TYPE_CALL), funcName(name), args(a) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        string ret = " CALL " + funcName + ": ";
        for (auto i : args)
        {
            ret += i->toString() + ", ";
        }
        return ret;
    };
};

class NBlock : public Node // block node, includes a series of statements covered by braces
{
public:
    vector<NStmt *> statements;
    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        string ret = "";
        for (auto i : statements)
        {
            ret += i->toString() + '\n';
        }
        return ret;
    };
};

class NIfStmt : public NStmt // if statement node
{
public:
    NExp *cond;
    NBlock *then, *el;

    NIfStmt(NExp *c, NBlock *t, NBlock *e)
        : NStmt(STMT_TYPE_IF), cond(c), then(t), el(e) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        return " IF \n" + cond->toString() + "\n  THEN \n" + then->toString() + "\n  ELSE \n" + el->toString();
    };
};

class NWhileStmt : public NStmt // while statement node
{
public:
    NExp *cond;
    NBlock *body;

    NWhileStmt(NExp *c, NBlock *b)
        : NStmt(STMT_TYPE_WHILE), cond(c), body(b) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        return " WHILE " + cond->toString() + "\n  " + body->toString();
    };
};

class NFuncDef : public NStmt // function definition node
{
public:
    string name;
    vector<pair<int, string>> args;
    NBlock *body;
    bool isExternal;

    NFuncDef(string n, vector<pair<int, string>> *a, NBlock *b)
        : NStmt(STMT_TYPE_FUNDEF), name(n), args(*a), body(b), isExternal(false) {}
    NFuncDef(string n, vector<pair<int, string>> *a)
        : NStmt(STMT_TYPE_FUNDEF), name(n), args(*a), body(nullptr), isExternal(true) {}
    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        string ret = " DEF " + name + ": ";
        for (auto i : args)
            ret += i.second + ", ";
        ret += "\n  " + body->toString();
        return ret;
    };
};

class NRetStmt : public NStmt // return statement node
{
public:
    NExp *retVal;
    NRetStmt(NExp *ret) : NStmt(STMT_TYPE_RET), retVal(ret) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        return " RETURN " + retVal->toString();
    }
};

#endif