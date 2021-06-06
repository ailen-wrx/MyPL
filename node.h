#ifndef _NODE_H
#define _NODE_H

#include <iostream>
#include <vector>
#include <string>
#include "codeGen.h"
#include "parser.hpp"

using namespace llvm;
using namespace std;

#define TYPE_VAR 1
#define TYPE_NUM 2
#define TYPE_STR 3
#define TYPE_ARR 4
#define TYPE_BINOP 5
#define TYPE_CALL 6
#define TYPE_ARRIDX 7

#define STMT_TYPE_EXP 11
#define STMT_TYPE_IF 12
#define STMT_TYPE_WHILE 13
#define STMT_TYPE_FUNDEF 14
#define STMT_TYPE_RET 15

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
    NStr(string v) : NExp(TYPE_STR), value(v) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override { return value; };
};

class NArray : public NExp
{
public:
    vector<NExp *> elements;
    NArray() : NExp(TYPE_ARR) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        string ret = "";
        for (auto i : elements)
        {
            ret += i->toString();
            return ret;
        }
    }
};

class NArrayIndex : public NExp
{
public:
    string arrName;
    NArrayIndex *super;
    NExp *index;

    NArrayIndex(NArrayIndex *s, NExp *idx) : NExp(TYPE_ARRIDX), arrName(""), super(s), index(idx) {}
    NArrayIndex(string name, NExp *idx) : NExp(TYPE_ARRIDX), arrName(name), super(NULL), index(idx) {}
    Value *codeGen(CodeGenContext &context) override;
    NExp *getTarget(CodeGenContext &context);
    void modify(CodeGenContext &context, NExp *newVal);
    string toString() override
    {
        return (arrName == string("") ? super->toString() : arrName) + " [ " + index->toString() + " ] ";
    }
};

class NBinOp : public NExp
{
public:
    enum yytokentype op;
    NExp *left, *right;
    NBinOp(enum yytokentype c, NExp *l, NExp *r) : NExp(TYPE_BINOP), op(c), left(l), right(r) {}

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

    NCallFunc(string name, vector<NExp *> a) : NExp(TYPE_CALL), funcName(name), args(a) {}

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
    NExp *Cond;
    NBlock *body;

    NWhileStmt(NExp *c, NBlock *b)
        : NStmt(STMT_TYPE_WHILE), Cond(c), body(b) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        return " WHILE " + Cond->toString() + "\n  " + body->toString();
    };
};

class NFuncDef : public NStmt
{
public:
    string name;
    vector<string> args;
    NBlock *body;

    NFuncDef(string n, vector<string> *a, NBlock *b)
        : NStmt(STMT_TYPE_FUNDEF), name(n), args(*a), body(b) {}
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