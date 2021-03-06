#ifndef BUS_HANDLER_H_
#define BUS_HANDLER_H_

/***********************************************************************/
/*************************** standard includes *************************/
/***********************************************************************/
#include <stdint.h>
#include <linux/can.h>
#include <sys/types.h>
#include <stdbool.h>
#include <linux/spi/spidev.h>

/***********************************************************************/
/************************** application includes ***********************/
/***********************************************************************/


/***********************************************************************/
/********************************* defines *****************************/
/***********************************************************************/

/*************************************************************************/
/********************************* enum **********************************/
/*************************************************************************/



/***********************************************************************/
/******************************** typedefs *****************************/
/***********************************************************************/
typedef struct _bus_handler
{

    int mBus;
    int mModeWrite;
    int mModeRead;
    uint8_t mBits;
    uint32_t mSpeed;
    uint16_t mDelay;
    struct spi_ioc_transfer mTransfer;
    uint8_t mTx[2];
    uint8_t mRx[1];
    uint8_t mDevice;

} bus_handler_t;



/*************************************************************************/
/********************* Class interface / public methods ******************/
/*************************************************************************/
/*---------------------------------------------------------------------------*/
/*! \brief
 *
 */
bool bus_init(bus_handler_t *object);

/*---------------------------------------------------------------------------*/
/*! \brief
 *
 */
bool bus_shutdown ();

/*---------------------------------------------------------------------------*/
/*! \brief
 *
 *
 *
 */
void writeReg(uint8_t reg,uint8_t val);
uint8_t readReg(uint8_t reg);
void setConf();
void getMagData(int16_t* data);


/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/


/*************************************************************************/
/****************************** Class Tasks *****************************/
/*************************************************************************/
#endif /* BUS_HANDLER_H_ */
