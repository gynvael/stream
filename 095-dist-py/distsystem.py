#!/usr/bin/python3
UNKNOWN = -1
MASTER = 0
WORKER = 1
WHOAMI = UNKNOWN

__all__ = [ "WHOAMI", "MASTER", "UNKNOWN", "WORKER" ]

object_map = {}

def _map_id(obj_id):
  if WHOAMI == MASTER:
    return obj_id

  return object_map.get(obj_id, None)






