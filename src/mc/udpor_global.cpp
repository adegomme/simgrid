/* Copyright (c) 2008-2021. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "udpor_global.hpp"
#include "xbt/log.h"

XBT_LOG_NEW_DEFAULT_SUBCATEGORY(mc_udpor_global, mc, "udpor_global");

namespace simgrid {
namespace mc {

bool EvtSetTools::contains(const EventSet events, const UnfoldingEvent* e)
{
  for (auto evt : events)
    if (*evt == *e)
      return true;
  return false;
}

} // namespace mc
} // namespace simgrid