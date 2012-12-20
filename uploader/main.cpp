/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#include "application.h"
#include "debug.h"
#include "system-tray.h"

int main(int argc, char **argv)
{
    ABC::Application app(argc, argv);

    ABC::SystemTray sysTray;
    sysTray.show();

    return app.exec();
}
