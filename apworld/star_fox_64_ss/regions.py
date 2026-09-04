from BaseClasses import Region

class StarFox64SSRegion(Region):
  pass

cache = {}

def create_region(world, region_name):
  if region_name in cache: return cache[region_name]
  region = StarFox64SSRegion(region_name, world.player, world.multiworld)
  cache[region_name] = region
  return region
