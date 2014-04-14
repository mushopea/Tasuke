// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// TODO: reference additional headers your program requires here

#define RETURN_WIDE_STRING(inputValue) std::wstringstream _s;	_s << inputValue; return _s.str()

#include <thread>
#include <QApplication>
#include <glog/logging.h>
#include "CppUnitTest.h"
#include "Tasuke.h"
#include "Storage.h"
#include "Commands.h"
#include "Interpreter.h"
#include "Exceptions.h"
#include "Constants.h"
#include "Task.h"
#include "TaskWindow.h"
#include "InputWindow.h"
#include "StorageStub.h"

namespace Microsoft { 
    namespace VisualStudio { 
        namespace CppUnitTestFramework {
			template<> static std::wstring ToString<QString>(const QString& string) {
				RETURN_WIDE_STRING(string.toStdString().data());
			}

			template<> static std::wstring ToString<QDate>(const QDate& date) {
				RETURN_WIDE_STRING(date.toString().toStdString().data());
			}

			template<> static std::wstring ToString<QTime>(const QTime& date) {
				RETURN_WIDE_STRING(date.toString().toStdString().data());
			}

			template<> static std::wstring ToString<QDateTime>(const QDateTime& date) {
				RETURN_WIDE_STRING(date.toString().toStdString().data());
			}
		}
	}
}

// Headers for CppUnitTest
#include "CppUnitTest.h"

#define MAX_TASKS 10