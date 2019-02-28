#ifndef SCIENCEPACKAGE_GLOBAL_H
#define SCIENCEPACKAGE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SCIENCEPACKAGE_LIBRARY)
#  define SCIENCEPACKAGESHARED_EXPORT Q_DECL_EXPORT
#else
#  define SCIENCEPACKAGESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SCIENCEPACKAGE_GLOBAL_H
