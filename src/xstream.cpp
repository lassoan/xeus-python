/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay and      *
* Wolf Vollprecht                                                          *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>

#include "xeus/xinterpreter.hpp"

#include "pybind11/embed.h"
#include "pybind11/functional.h"
#include "pybind11/pybind11.h"

#include "xeus-python/xeus_python_config.hpp"

#include "xstream.hpp"

namespace py = pybind11;

namespace xpyt
{
    xstream::xstream(std::string stream_name)
        : m_stream_name(stream_name)
    {
    }

    xstream::~xstream()
    {
    }

    void xstream::write(const std::string& message)
    {
        xeus::get_interpreter().publish_stream(m_stream_name, message);
    }

    XEUS_PYBIND_MODULE(xeus_python_stream, m)
    {
        py::class_<xstream>(m, "XPythonStream")
            .def(py::init<std::string>())
            .def("write", &xstream::write);
    }
}
