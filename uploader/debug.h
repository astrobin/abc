/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_DEBUG_H
#define ABC_DEBUG_H

#include <QDebug>

/* TODO: make this configurable */
#define DEBUG()    qDebug() << Q_FUNC_INFO

#endif /* ABC_DEBUG_H */
