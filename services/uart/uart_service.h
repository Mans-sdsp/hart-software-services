#ifndef HSS_UART_SERVICE_H
#define HSS_UART_SERVICE_H

/*******************************************************************************
 * Copyright 2019-2021 Microchip Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *
 * Hart Software Services - Virtual UART API
 *
 */


/*!
 * \file Virtual UART API
 * \brief UART Driver State Machine API function declarations
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "hss_state_machine.h"
#include "hss_debug.h"

enum IPIStatusCode HSS_UartTx_IPIHandler(TxId_t transaction_id, enum HSSHartId source, uint32_t immediate_arg, void *p_extended_buffer_in_ddr);
enum IPIStatusCode HSS_UartPollRx_IPIHandler(TxId_t transaction_id, enum HSSHartId source, uint32_t immediate_arg, void *p_extended_buffer_in_ddr);

extern struct StateMachine uart_service;

#ifdef __cplusplus
}
#endif

#endif
