#include "sga_fwbro.h"

NAN_MODULE_INIT(Init) {
    Nan::Set(target, Nan::New("countResult").ToLocalChecked(),
        Nan::GetFunction(Nan::New<v8::FunctionTemplate>(countResult)).ToLocalChecked());
}

NODE_MODULE(sga_fwbro, Init)