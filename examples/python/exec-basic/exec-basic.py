# Copyright (c) 2018. The SimGrid Team. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the license (GNU LGPL) which comes with this package.

import sys
from simgrid import *

def executor():
    # execute() tells SimGrid to pause the calling actor until
    # its host has computed the amount of flops passed as a parameter
    this_actor.execute(98095)
    this_actor.info("Done.")
    # This simple example does not do anything beyond that

def privileged():
    #  This version of execute() with two parameters specifies that this execution
    # gets a larger share of the resource.
    #
    # Since the priority is 2, it computes twice as fast as a regular one.
    #
    # So instead of a half/half sharing between the two executions,
    # we get a 1/3 vs 2/3 sharing.
    this_actor.execute(98095, 2);
    this_actor.info("Done.");

    # Note that the timings printed when executing this example are a bit misleading,
    # because the uneven sharing only last until the privileged actor ends.
    # After this point, the unprivileged one gets 100% of the CPU and finishes
    # quite quickly.

i = 0
if "--" in sys.argv:
    i = sys.argv.index("--")
e = Engine(sys.argv[0:i])
e.load_platform(sys.argv[i+1])

Actor.create("executor", Host.by_name("Tremblay"), executor)
Actor.create("privileged", Host.by_name("Tremblay"), privileged)

e.run()