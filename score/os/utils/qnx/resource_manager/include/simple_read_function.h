/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_LIB_OS_SIMPLE_READ_FUNCTION_H
#define SCORE_LIB_OS_SIMPLE_READ_FUNCTION_H

#include "score/os/qnx/types/i_open_function.h"

#include "score/os/qnx/iofunc.h"
#include "score/os/qnx/resmgr.h"

#include "score/result/result.h"

namespace score
{
namespace os
{

// class to provide interface and functionality for simple reading
class SimpleReadFunction : public IReadFunction
{
  public:
    SimpleReadFunction(ResMgr& resmgr, IoFunc& iofunc);
    int operator()(resmgr_context_t* ctp, io_read_t* msg, RESMGR_OCB_T* ocb) override;
    // get offset if it is possible if not return error
    // in case of error return error code according to
    // https://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.lib_ref/topic/e/errno.html
    static score::Result<off_t> getOffset(const io_read_t* msg, const RESMGR_OCB_T* ocb);

  private:
    // pure function to be called in () operator to read data from the source
    // in case of error return error code according to
    // https://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.lib_ref/topic/e/errno.html
    virtual score::Result<std::size_t> read(const off_t offset, const size_t nbytes, std::uint64_t& result) = 0;
    // to check and iniliaze the source of data to read data
    // in case of error return error code according to
    // https://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.lib_ref/topic/e/errno.html
    virtual score::Result<void> preRead(off_t, std::size_t)
    {
        return {};
    }
    // to de-initializate the source of data
    // in case of error return error code according to
    // https://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.lib_ref/topic/e/errno.html
    virtual score::Result<void> postRead()
    {
        return {};
    }

    ResMgr& resmgr_;
    IoFunc& iofunc_;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SIMPLE_READ_FUNCTION_H
