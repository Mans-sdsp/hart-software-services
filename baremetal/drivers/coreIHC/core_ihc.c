/*******************************************************************************
 * Copyright 2021-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * PolarFire SoC Microprocessor Subsystem Inter-Hart Communication bare metal software driver
 * implementation.
 *
 */
#include <string.h>
#include <stdio.h>
#include "mpfs_hal/mss_hal.h"
#include "core_ihc.h"
#include "drivers/mss_uart/mss_uart.h"

/******************************************************************************/
/* configuration           arrays populated from user defines                 */
/******************************************************************************/

IHC_TypeDef             IHC_H0_IP_GROUP ;
IHC_TypeDef             IHC_H1_IP_GROUP ;
IHC_TypeDef             IHC_H2_IP_GROUP ;
IHC_TypeDef             IHC_H3_IP_GROUP ;
IHC_TypeDef             IHC_H4_IP_GROUP ;

IHC_TypeDef * IHC[]={ &IHC_H0_IP_GROUP , &IHC_H1_IP_GROUP, &IHC_H2_IP_GROUP, &IHC_H3_IP_GROUP, &IHC_H4_IP_GROUP};

/**
 * \brief IHC configuration
 *
 */
const uint64_t ihc_base_addess[][5U] = {
        /* hart 0 */
        {0x0,
        IHC_LOCAL_H0_REMOTE_H1,
        IHC_LOCAL_H0_REMOTE_H2,
        IHC_LOCAL_H0_REMOTE_H3,
        IHC_LOCAL_H0_REMOTE_H4},
        /* hart 1 */
        {IHC_LOCAL_H1_REMOTE_H0,
        0x0,
        IHC_LOCAL_H1_REMOTE_H2,
        IHC_LOCAL_H1_REMOTE_H3,
        IHC_LOCAL_H1_REMOTE_H4},
        /* hart 2 */
        {IHC_LOCAL_H2_REMOTE_H0,
        IHC_LOCAL_H2_REMOTE_H1,
        0x0,
        IHC_LOCAL_H2_REMOTE_H3,
        IHC_LOCAL_H2_REMOTE_H4},
        /* hart 3 */
        {IHC_LOCAL_H3_REMOTE_H0,
        IHC_LOCAL_H3_REMOTE_H1,
        IHC_LOCAL_H3_REMOTE_H2,
        0x0,
        IHC_LOCAL_H3_REMOTE_H4},
        /* hart 4 */
        {IHC_LOCAL_H4_REMOTE_H0,
        IHC_LOCAL_H4_REMOTE_H1,
        IHC_LOCAL_H4_REMOTE_H2,
        IHC_LOCAL_H4_REMOTE_H3,
        0x0},
};

/**
 * \brief IHC configuration
 *
 */
const uint64_t ihca_base_addess[5U] = {

        IHCA_LOCAL_H0,
        IHCA_LOCAL_H1,
        IHCA_LOCAL_H2,
        IHCA_LOCAL_H3,
        IHCA_LOCAL_H4
};

/**
 * \brief Remote harts connected via channel to a local hart
 *
 */
const uint32_t ihca_remote_harts[5U] = {
        IHCA_H0_REMOTE_HARTS,
        IHCA_H1_REMOTE_HARTS,
        IHCA_H2_REMOTE_HARTS,
        IHCA_H3_REMOTE_HARTS,
        IHCA_H4_REMOTE_HARTS
};


/**
 * \brief Remote harts connected via channel to a local hart
 *
 */
const uint32_t ihca_remote_hart_ints[5U] = {
        IHCA_H0_REMOTE_HARTS_INTS,
        IHCA_H1_REMOTE_HARTS_INTS,
        IHCA_H2_REMOTE_HARTS_INTS,
        IHCA_H3_REMOTE_HARTS_INTS,
        IHCA_H4_REMOTE_HARTS_INTS
};

mss_ihc_plex_instance my_instance;

/******************************************************************************/
/* Private Functions                                                          */
/***********************************************************************8888***/
static uint32_t IHCA_parse_incoming_hartid(uint32_t my_hart_id, bool *is_ack, bool polling);

/**************************************************************************/
/* Public API Functions                                                       */
/**************************************************************************/

/**
 * Init IHC Module
 * This must be called from monitor core ( needs access to all cores/registers )
 * It function is called before local init functions are called.
 * @param none
 * @return none
 */
void IHC_global_init(void)
{
    uint32_t remote_hart_id;
    uint32_t my_hart_id = 0;

    while(my_hart_id < 5U)
    {
        remote_hart_id = 0U;

        while(remote_hart_id < 5U)
        {
            IHC[my_hart_id]->local_h_setup.msg_in_handler[remote_hart_id] = NULL;
            /*
             * Configure base addresses
             */
            IHC[my_hart_id]->HART_IHC[remote_hart_id] = (IHC_IP_TypeDef *)ihc_base_addess[my_hart_id][remote_hart_id];
            IHC[my_hart_id]->HART_IHC[remote_hart_id]->CTR_REG.CTL_REG = 0U;
            remote_hart_id++;
        }
        /*
         * Configure base addresses
         */
        IHC[my_hart_id]->interrupt_concentrator = (IHCA_IP_TypeDef *)ihca_base_addess[my_hart_id];
        /*
         *
         */
        IHC[my_hart_id]->interrupt_concentrator->INT_EN.INT_EN          = 0x0U;
        my_hart_id++;
    }
}

/**
 * This is called from all the local harts using CORE IHC
 * @param handler
 * @return
 */
uint8_t IHCA_local_context_init(uint32_t hart_to_configure)
{
    uint8_t result = false;

    (void)result;

    {
        /*
         * Configure the base addresses in this hart context
         *
         */
        uint32_t remote_hart_id = 0U;

        while(remote_hart_id < 5U)
        {
            IHC[hart_to_configure]->local_h_setup.msg_in_handler[remote_hart_id] = NULL;
            /*
             * Configure base addresses
             */
            IHC[hart_to_configure]->HART_IHC[remote_hart_id] = (IHC_IP_TypeDef *)ihc_base_addess[hart_to_configure][remote_hart_id];
            IHC[hart_to_configure]->HART_IHC[remote_hart_id]->CTR_REG.CTL_REG = 0U;
            remote_hart_id++;
        }
        /*
         * Configure base addresses
         */
        IHC[hart_to_configure]->interrupt_concentrator = (IHCA_IP_TypeDef *)ihca_base_addess[hart_to_configure];
        /*
         *
         */
        IHC[hart_to_configure]->interrupt_concentrator->INT_EN.INT_EN          = 0x0U;
    }


    IHC[hart_to_configure]->local_h_setup.connected_harts = ihca_remote_harts[hart_to_configure];
    IHC[hart_to_configure]->local_h_setup.connected_hart_ints = ihca_remote_hart_ints[hart_to_configure];

    return(true);
}


uint8_t IHCA_local_remote_config(uint32_t hart_to_configure, uint32_t remote_hart_id, QUEUE_IHC_INCOMING  handler, bool set_mpie_en, bool set_ack_en )
{
    uint8_t result = false;

    (void)result;

    /*
     * Set-up enables in concentrator
     */
    IHC[hart_to_configure]->local_h_setup.msg_in_handler[remote_hart_id] = handler;

    if(handler != NULL)
    {
    	IHC[hart_to_configure]->interrupt_concentrator->INT_EN.INT_EN = ihca_remote_hart_ints[hart_to_configure];
    }

    {
        if (set_mpie_en == true)
        {
            IHC[hart_to_configure]->HART_IHC[remote_hart_id]->CTR_REG.CTL_REG |= MPIE_EN;
        }
        if(set_ack_en == true)
        {
            IHC[hart_to_configure]->HART_IHC[remote_hart_id]->CTR_REG.CTL_REG |= ACKIE_EN;
        }
    }

    return(true);
}

#define IHC_INCOMING_HANDLER example_incoming_handler
uint32_t example_incoming_handler(uint32_t remote_hart_id,  uint32_t * incoming_msg)
{

    (void)remote_hart_id;
    (void)incoming_msg;
    return(0U);
}

/**
 * Send a message to another hart using CoreIHC
 * @param remote_hart_id
 * @param message
 * @return MP_BUSY / MESSAGE_SENT
 */
uint32_t IHC_tx_message(IHC_CHANNEL channel, uint32_t *message)
{
    uint32_t i, ret_value = NO_MESSAGE_RX;

    uint32_t my_hart_id = context_to_local_hart_id(channel);
    uint32_t remote_hart_id = context_to_remote_hart_id(channel);
    uint32_t message_size = IHC[my_hart_id]->HART_IHC[remote_hart_id]->size_msg;

    /*
     * return if RMP bit 1 indicating busy
     */
    if (RMP_MESSAGE_PRESENT == (IHC[my_hart_id]->HART_IHC[remote_hart_id]->CTR_REG.CTL_REG & RMP_MASK))
    {
        ret_value = MP_BUSY;
    }
    else if (ACK_INT_MASK == (IHC[my_hart_id]->HART_IHC[remote_hart_id]->CTR_REG.CTL_REG & ACK_INT_MASK))
    {
        ret_value = MP_BUSY;
    }
    else
    {
        /*
         * Fill the buffer
        */
        for(i = 0;i < message_size; i++)
        {
            IHC[my_hart_id]->HART_IHC[remote_hart_id]->mesg_out[i] = message[i];
        }
        /*
         * set the MP bit. This will notify other of incoming hart message
         */
        IHC[my_hart_id]->HART_IHC[remote_hart_id]->CTR_REG.CTL_REG |= RMP_MESSAGE_PRESENT;
        /*
         * report status
         */
        ret_value = MESSAGE_SENT;
    }

    return (ret_value);
}

/**
 * Called by application ( eg rp message )
 * @param remote_hart_id
 * @param handle_incoming This is a point to a function that is provided by
 * upper layer. It will read/copy the incoming message.
 * @return
 */
uint32_t IHC_rx_message(IHC_CHANNEL channel, QUEUE_IHC_INCOMING handle_incoming, bool is_ack, uint32_t * message_storage_ptr)
{

    uint32_t ret_value = NO_MESSAGE_RX;
    uint64_t my_hart_id = read_csr(mhartid);
    uint32_t remote_hart_id = context_to_remote_hart_id(channel);
    uint32_t message_size = IHC[my_hart_id]->HART_IHC[remote_hart_id]->size_msg;

    if (is_ack == true)
    {
        handle_incoming(remote_hart_id, (uint32_t *)&IHC[my_hart_id]->HART_IHC[remote_hart_id]->mesg_in[0U], message_size, is_ack, message_storage_ptr);
    }
    else if (MP_MESSAGE_PRESENT == (IHC[my_hart_id]->HART_IHC[remote_hart_id]->CTR_REG.CTL_REG & MP_MASK))
    {
        /*
         * check if we have a message
         */
        handle_incoming(remote_hart_id, (uint32_t *)&IHC[my_hart_id]->HART_IHC[remote_hart_id]->mesg_in[0U], message_size, is_ack, message_storage_ptr);
        {
            /*
             * set MP to 0
             * Note this generates an interrupt on the other hart if it has RMPIE
             * bit set in the control register
             */

            volatile uint32_t temp = IHC[my_hart_id]->HART_IHC[remote_hart_id]->CTR_REG.CTL_REG & ~MP_MASK;
            /* Check if ACKIE_EN is set*/
            if(temp & ACKIE_EN)
            {
                temp |= ACK_INT;
            }
            IHC[my_hart_id]->HART_IHC[remote_hart_id]->CTR_REG.CTL_REG = temp;

            ret_value = MESSAGE_RX;
        }
    }
    else
    {
        /*
         * report status
         */
        ret_value = NO_MESSAGE_RX;
    }

    return (ret_value);
}

/*******************************************************************************
 * Interrupt support functions
 ******************************************************************************/

/**
 *
 */
void  message_present_isr(void)
{
    bool is_ack;
    uint64_t my_hart_id = read_csr(mhartid);
    /*
     * Check all our channels
     */
	uint32_t origin_hart = IHCA_parse_incoming_hartid((uint32_t)my_hart_id, &is_ack, false);
	if(origin_hart != 99U)
	{
		/*
		 * process incoming packet
		 */
	    IHC_rx_message(origin_hart, IHC[my_hart_id]->local_h_setup.msg_in_handler[origin_hart], is_ack, NULL );
        if(is_ack == true)
        {
            /* clear the ack */
            IHC[my_hart_id]->HART_IHC[origin_hart]->CTR_REG.CTL_REG &= ~ACK_CLR;
        }
	}
}

/**
 *
 */
void  message_present_indirect_isr(uint32_t my_hart_id, uint32_t remote_channel, uint32_t * message_storage_ptr)
{
    bool is_ack;

    (void)remote_channel;

    /*
     * Check all our channels
     */
    uint32_t origin_hart = IHCA_parse_incoming_hartid((uint32_t)my_hart_id, &is_ack, false);
    if(origin_hart != 99U)
    {
        /*
         * process incoming packet
         */
        IHC_rx_message(origin_hart, IHC[my_hart_id]->local_h_setup.msg_in_handler[origin_hart], is_ack, message_storage_ptr );
        if(is_ack == true)
        {
            /* clear the ack */
            IHC[my_hart_id]->HART_IHC[origin_hart]->CTR_REG.CTL_REG &= ~ACK_CLR;
        }
    }
}


void  message_present_poll(void)
{
    bool is_ack;
    uint64_t my_hart_id = read_csr(mhartid);
    /*
     * Check all our channels
     */
    uint32_t origin_hart = IHCA_parse_incoming_hartid((uint32_t)my_hart_id, &is_ack, true);
    if(origin_hart != 99U)
    {
        /*
         * process incoming packet
         */
        IHC_rx_message(origin_hart, IHC[my_hart_id]->local_h_setup.msg_in_handler[origin_hart], is_ack, NULL );
        if(is_ack == true)
        {
            /* clear the ack */
            IHC[my_hart_id]->HART_IHC[origin_hart]->CTR_REG.CTL_REG &= ~ACK_CLR;
        }
    }

    /*
     * clear the interrupt
     */
}

/*******************************************************************************
 * local functions
 ******************************************************************************/

/**
 * Check where the message is coming from
 * @return returns hart ID of incoming message
 */
static uint32_t IHCA_parse_incoming_hartid(uint32_t my_hart_id, bool *is_ack, bool polling)
{

    uint32_t hart_id = 0U;
    uint32_t return_hart_id = 99U;

    while(hart_id < 5U)
    {
        if (IHC[my_hart_id]->local_h_setup.connected_harts & (0x01U << hart_id))
        {
            uint32_t test_int = (0x01U << ((hart_id * 2) + 1));
            if(IHC[my_hart_id]->interrupt_concentrator->MSG_AVAIL_STAT.MSG_AVAIL & test_int)
            {
                if (polling == true)
                {
                    return_hart_id = hart_id;
                    *is_ack = true;
                    break;
                }
                else if(IHC[my_hart_id]->local_h_setup.connected_hart_ints & test_int)
                {
                    return_hart_id = hart_id;
                    *is_ack = true;
                    break;
                }
            }
            test_int = (0x01U << (hart_id * 2));
            if(IHC[my_hart_id]->interrupt_concentrator->MSG_AVAIL_STAT.MSG_AVAIL & test_int)
            {
                if (polling == true)
                {
                    return_hart_id = hart_id;
                    *is_ack = false;
                    break;
                }
                else if(((IHC[my_hart_id]->local_h_setup.connected_hart_ints & test_int) == test_int ) )
                {
                    return_hart_id = hart_id;
                    *is_ack = false;
                    break;
                }
            }
        }
        hart_id++;
    }
    return(return_hart_id);
}

/**
 * Returns remote hart ID
 * @param channel
 * @return
 */
uint32_t context_to_local_hart_id(IHC_CHANNEL channel)
{
    uint32_t hart = 0xFFU;
    uint32_t hart_idx = 0U;
    uint32_t harts_in_context = LIBERO_SETTING_CONTEXT_B_HART_EN;
    uint64_t my_hart_id = read_csr(mhartid);

    /*
     * If we are sending to a Context, assume we are a Context.
     * i.e. HSS will not send directly to a contect
     */
    if(channel <= IHC_CHANNEL_TO_HART4)
    {
        hart = (uint32_t)my_hart_id;
    }
    else
    {
        if(channel == IHC_CHANNEL_TO_CONTEXTA)
        {
            /* we are context B */
            harts_in_context = LIBERO_SETTING_CONTEXT_B_HART_EN;
        }
        else
        {
            /* we are context A */
            harts_in_context = LIBERO_SETTING_CONTEXT_A_HART_EN;
        }

        hart_idx = 0U;
        while(hart_idx < 5U)
        {
            if  (harts_in_context & (1U<<hart_idx))
            {
                hart = hart_idx;
                break;
            }
            hart_idx++;
        }
    }

    return (hart);
}

uint32_t context_to_remote_hart_id(IHC_CHANNEL channel)
{
    uint32_t hart = 0xFFU;
    uint32_t hart_idx = 0U;
    uint32_t harts_in_context = LIBERO_SETTING_CONTEXT_B_HART_EN;

    if(channel <= IHC_CHANNEL_TO_HART4)
    {
        hart = channel;
    }
    else
    {
#ifndef LIBERO_SETTING_CONTEXT_A_HART_EN
#error "Use newer mss configurator to configure"
#else
        ASSERT(LIBERO_SETTING_CONTEXT_A_HART_EN > 0U);
        ASSERT(LIBERO_SETTING_CONTEXT_B_HART_EN > 0U);
        /* Determine context we are in */

        if(channel == IHC_CHANNEL_TO_CONTEXTA)
        {

           harts_in_context = LIBERO_SETTING_CONTEXT_A_HART_EN;
        }
        else
        {
            harts_in_context = LIBERO_SETTING_CONTEXT_B_HART_EN;
        }

        hart_idx = 0U;
        while(hart_idx < 5U)
        {
            if  (harts_in_context & (1U<<hart_idx))
            {
                hart = hart_idx;
                break;
            }
            hart_idx++;
        }
#endif
    }


    return (hart);
}



uint8_t core_ihca_hart0_int(void)
{
    message_present_isr();
    return(EXT_IRQ_KEEP_ENABLED);
}
uint8_t core_ihca_hart1_int(void)
{
    message_present_isr();
    return(EXT_IRQ_KEEP_ENABLED);
}
uint8_t core_ihca_hart2_int(void)
{
    message_present_isr();
    return(EXT_IRQ_KEEP_ENABLED);
}
uint8_t core_ihca_hart3_int(void)
{
    message_present_isr();
    return(EXT_IRQ_KEEP_ENABLED);
}
uint8_t core_ihca_hart4_int(void)
{
    message_present_isr();
    return(EXT_IRQ_KEEP_ENABLED);
}
