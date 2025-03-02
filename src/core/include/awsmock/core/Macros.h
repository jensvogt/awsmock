//
// Created by vogje01 on 02/03/2025.
//

#ifndef AWSMOCK_CORE_WINDOWS_H
#define AWSMOCK_CORE_WINDOWS_H

#ifdef WIN32
/*
 * Microsoft Visual C
 */
#ifdef AWSMOCK_EXPORTS
#define AWSMOCK_API __declspec(dllexport)
#else
#define AWSMOCK_API __declspec(dllimport)
#endif

#endif

#endif//AWSMOCK_CORE_WINDOWS_H
