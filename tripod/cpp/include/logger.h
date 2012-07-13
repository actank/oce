/*
 * logger.h
 *
 *  Created on: 2011-3-21
 *      Author: guanghe.ge
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#define MCE_WARN(msg) \
 do { \
        std::cout << msg << std::endl; \
    } while(0);

#define MCE_DEBUG(msg) \
 do { \
        std::cout << msg << std::endl; \
    } while(0);

#define MCE_INFO(msg) \
 do { \
        std::cout << msg << std::endl; \
    } while(0);

#endif /* LOGGER_H_ */
