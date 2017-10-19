#include "sga_fwbro.h"
#include <vector>
#include "fwbro.h"

// countResult(arr, paytables)
void countResult(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() != 3) {
        Nan::ThrowTypeError("Wrong number of arguments");

        return;
    }

    if (!info[0]->IsArray() || !info[1]->IsArray() || !info[2]->IsBoolean()) {
        Nan::ThrowTypeError("Wrong arguments");

        return;
    }

    v8::Local<v8::Array> arr = info[0].As<v8::Array>();
    if (arr->Length() != 5) {
        Nan::ThrowTypeError("Wrong arr");
        
        return;
    }

    // v8::Local<v8::Array> narr[5];
    int symbolarr[5][5];

    for (int i = 0; i < 5; ++i) {
        if (!arr->Get(i)->IsArray()) {
            Nan::ThrowTypeError("Wrong arr[]");
    
            return;
        }

        v8::Local<v8::Array> ca = arr->Get(i).As<v8::Array>();
        if (ca->Length() != 5) {
            Nan::ThrowTypeError("Wrong arr[][]");
            
            return;
        }

        for (int j = 0; j < 5; ++j) {
            if (!ca->Get(j)->IsNumber()) {
                Nan::ThrowTypeError("Wrong arr[][] type");
        
                return;
            }

            symbolarr[i][j] = ca->Get(j)->NumberValue();
        }
    }

    v8::Local<v8::Array> arrpaytables = info[1].As<v8::Array>();
    std::vector<PaytablesNode> lstPaytables;
    // std::vector< v8::Local<v8::Array> > lstp;

    for (int i = 0; i < (int)arrpaytables->Length(); ++i) {
        if (!arrpaytables->Get(i)->IsObject()) {
            Nan::ThrowTypeError("Wrong paytables[]");
    
            return;
        }

        v8::Local<v8::Array> ca = arrpaytables->Get(i).As<v8::Array>();
        if (ca->Length() != 5) {
            Nan::ThrowTypeError("Wrong paytables[][]");
            
            return;
        }

        PaytablesNode cn;

        cn.symbol = i;
        for (int j = 0; j < 5; ++j) {
            if (!ca->Get(j)->IsNumber()) {
                Nan::ThrowTypeError("Wrong paytables[][] type");
        
                return;
            }

            cn.payout[j] = ca->Get(j)->NumberValue();
        }

        lstPaytables.push_back(cn);
    }

    int payout = procGameScene(symbolarr, lstPaytables, info[2]->BooleanValue());

    v8::Local<v8::Number> num = Nan::New<v8::Number>(payout);
    info.GetReturnValue().Set(num);
}