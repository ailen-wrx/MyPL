#include "builtin.h"

map<string, Value *(*)(CodeGenContext &context, vector<NExp *> &args)> BuiltinFunction;

Value *BuiltinPrint(CodeGenContext &context, vector<NExp *> &args)
{
    // for (NExp *i : this->args)
    //     {
    //         switch (i->type)
    //         {
    //         case TYPE_NUM:
    //         {
    //             cout << ((NNum *)i)->value;
    //             break;
    //         }

    //         case TYPE_STR:
    //         {
    //             cout << ((NStr *)i)->value;
    //             break;
    //         }

    //         case TYPE_VAR:
    //         {
    //             NExp *val = ((NVariable *)i)->getTarget(context);
    //             switch (val->type)
    //             {
    //             case TYPE_NUM:
    //             {
    //                 cout << ((NNum *)val)->value;
    //                 break;
    //             }
    //             case TYPE_STR:
    //             {
    //                 cout << ((NStr *)val)->value;
    //                 break;
    //             }
    //             default:
    //             {
    //                 Value *v = i->codeGen(context);
    //                 cout << (((ConstantFP *)v)->getValue()).convertToDouble();
    //             }
    //             }
    //             break;
    //         }

    //         default:
    //             Value *v = i->codeGen(context);
    //             cout << (((ConstantFP *)v)->getValue()).convertToDouble();
    //         }
    //     }
    //     cout << endl;
    return nullptr;
}
Value *BuiltinReadn(CodeGenContext &context, vector<NExp *> &args)
{
    //double in;
    //     cin >> in;
    //     NExp *target = this->args[0];
    //     switch (target->type)
    //     {
    //     case TYPE_VAR:
    //         context.vars[((NVariable *)target)->name] = new NNum(in);
    //         break;
    //     case TYPE_ARRIDX:
    //         ((NArrayIndex *)target)->modify(context, new NNum(in));
    //         break;
    //     default:
    //         cout << "Invalid assignment" << endl;
    //     }
    return nullptr;
}
Value *BuiltinReads(CodeGenContext &context, vector<NExp *> &args)
{
    //     string in;
    //     cin >> in;
    //     NExp *target = this->args[0];
    //     switch (target->type)
    //     {
    //     case TYPE_VAR:
    //         context.vars[((NVariable *)target)->name] = new NStr(in);
    //         break;
    //     case TYPE_ARRIDX:
    //         ((NArrayIndex *)target)->modify(context, new NStr(in));
    //         break;
    //     default:
    //         cout << "Invalid assignment" << endl;
    //     }
    return nullptr;
}

void initializeBuiltinFunction()
{
    BuiltinFunction["print"] = BuiltinPrint;
    BuiltinFunction["readn"] = BuiltinReadn;
    BuiltinFunction["reads"] = BuiltinReads;
}