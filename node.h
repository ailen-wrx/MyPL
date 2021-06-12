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

#define TYPE_VAR 1
#define TYPE_INT 2
#define TYPE_DOUBLE 3
#define TYPE_STR 4
#define TYPE_ARR 5
#define TYPE_BINOP 6
#define TYPE_CALL 7
#define TYPE_ARRIDX 8
#define TYPE_IDENTIFIER 9

#define STMT_TYPE_EXP 11
#define STMT_TYPE_IF 12
#define STMT_TYPE_WHILE 13
#define STMT_TYPE_FUNDEF 14
#define STMT_TYPE_RET 15

class CodeGenContext;

class Node
{
public:
    virtual Value *codeGen(CodeGenContext &context) = 0;
    virtual string toString() = 0;
};

class NStmt : public Node
{
public:
    int stmt_type;
    NStmt(int t) : stmt_type(t) {}
};

class NExp : public NStmt
{
public:
    int type;
    NExp(int t) : NStmt(STMT_TYPE_EXP), type(t) {}
};

class NVariable : public NExp
{
public:
    string name;
    NVariable(string s) : NExp(TYPE_VAR), name(s) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return name; };
};

class NNum : public NExp
{
public:
    double value;
    NNum(double v) : NExp(TYPE_NUM), value(v) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return to_string(value); };
};

class NStr : public NExp
{
public:
    string value;
    NStr(string v) : NExp(TYPE_STR), value(v.substr(1, v.length() - 2)) {}
    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return value; };
};

class NArray : public NExp
{
public:
    vector<Value *> elements;
    int size;
    int elementType;
    NArray(int s) : NExp(TYPE_ARR), size(s), elementType(-1) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return ""; }
};

class NArrayIndex : public NExp
{
public:
    string arrName;
    NArray *array;
    NExp *index;

    NArrayIndex(string name, NExp *idx) : NExp(TYPE_ARRIDX), arrName(name), array(nullptr), index(idx) {}
    Value *codeGen(CodeGenContext &context) override;
    NArray *getArrayNode(CodeGenContext &context);
    Value *modify(CodeGenContext &context, NExp *newVal);
    string toString() override
    {
        return arrName + " [ " + index->toString() + " ] ";
    }
};

class NBinOp : public NExp
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
};

class NCallFunc : public NExp
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

class NBlock : public Node
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

class NIfStmt : public NStmt
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

class NWhileStmt : public NStmt
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

class NFuncDef : public NStmt
{
public:
    string name;
    vector<string> args;
    NBlock *body;
    bool isExternal;

    NFuncDef(string n, vector<string> *a, NBlock *b)
        : NStmt(STMT_TYPE_FUNDEF), name(n), args(*a), body(b), isExternal(false) {}
    NFuncDef(string n, vector<string> *a)
        : NStmt(STMT_TYPE_FUNDEF), name(n), args(*a), body(nullptr), isExternal(true) {}
    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        string ret = " DEF " + name + ": ";
        for (auto i : args)
            ret += i + ", ";
        ret += "\n  " + body->toString();
        return ret;
    };
};

class NRetStmt : public NStmt
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