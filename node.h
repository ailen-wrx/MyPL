#ifndef _NODE_H
#define _NODE_H

#include <iostream>
#include <vector>
#include <string>
#include "codeGen.h"

using namespace llvm;
using namespace std;

#define TYPE_VAR 1
#define TYPE_NUM 2
#define TYPE_STR 3
#define TYPE_ARR 4
#define TYPE_BINOP 5
#define TYPE_CALL 6

class Node
{
public:
    virtual Value *codeGen(CodeGenContext &context) = 0;
    virtual string toString() = 0;
};

class NStmt : public Node
{
};

class NExp : public NStmt
{
public:
    int type;
    NExp(int t) : type(t) {}
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

class NBinOp : public NExp
{
public:
    char op;
    NExp *left, *right;
    NBinOp(char c, NExp *l, NExp *r) : NExp(TYPE_BINOP), op(c), left(l), right(r) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        return left->toString() + " " + op + " " + right->toString();
    };
};

class NCallFunc : public NExp
{
public:
    string funcName;
    vector<NExp *> args;

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

    NIfStmt(NExp *c, NBlock *t, NBlock *e) : cond(c), then(t), el(e) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        return " IF \n" + cond->toString() + "\n  THEN \n" + then->toString() + "\n  ELSE \n" + el->toString();
    };
};

class NForStmt : public NStmt
{
public:
    string loopVar;
    double start, end, step;
    NBlock *body;

    NForStmt(string var, double s, double e, double t, NBlock *b)
        : loopVar(var), start(s), end(e), step(t), body(b) {}

    Value *codeGen(CodeGenContext &context) override;
    string toString() override
    {
        return " FOR " + loopVar + " FROM " + to_string(start) + " TO " +
               to_string(end) + " STEP " + to_string(step) + "\n  " + body->toString();
    };
};

class NFuncDef : public NStmt
{
public:
    string name;
    vector<string> args;
    NBlock *body;

    NFuncDef(string n, vector<string> &a, NBlock *b) : name(n), args(a), body(b) {}
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

#endif