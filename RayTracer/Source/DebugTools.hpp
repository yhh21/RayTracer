#pragma once

#include "pch.h"
#include <string>
#include <sstream>
#include <iostream>

#include <windows.h>
#include <dbgHelp.h>
#include <processthreadsapi.h>

// ��Ӷ�dbghelp.lib�ı�������
#pragma comment(lib, "dbghelp.lib")


using namespace std;

namespace Common {
    namespace DebugTools {
        /// copy from https://blog.csdn.net/windpenguin/article/details/80382344
        static string TraceStack() {
            static const int MAX_STACK_FRAMES = 5;

            void *pStack[MAX_STACK_FRAMES];

            HANDLE process = GetCurrentProcess();
            SymInitialize(process, NULL, TRUE);
            WORD frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, pStack, NULL);

            std::ostringstream oss;
            oss << "stack traceback: " << std::endl;
            for (WORD i = 0; i < frames; ++i) {
                DWORD64 address = (DWORD64)(pStack[i]);

                DWORD64 displacementSym = 0;
                char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
                PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
                pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                pSymbol->MaxNameLen = MAX_SYM_NAME;

                DWORD displacementLine = 0;
                IMAGEHLP_LINE64 line;
                //SymSetOptions(SYMOPT_LOAD_LINES);
                line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

                if (SymFromAddr(process, address, &displacementSym, pSymbol)
                    && SymGetLineFromAddr64(process, address, &displacementLine, &line)) {
                    oss << "\t" << pSymbol->Name << " at " << line.FileName << ":" << line.LineNumber << "(0x" << std::hex << pSymbol->Address << std::dec << ")" << std::endl;
                }
                else {
                    oss << "\terror: " << GetLastError() << std::endl;
                }
            }
            return oss.str();
        }


        static void PrintDebugLog(const char *str, bool isPrintTraceStack = true) {
            cout << str;
            if (isPrintTraceStack) {
                cout << TraceStack();
            }
        }

        static void PrintDebugLog(const string str, bool isPrintTraceStack = true) {
            cout << str;
            if (isPrintTraceStack) {
                cout << TraceStack();
            }
        }
    }
}