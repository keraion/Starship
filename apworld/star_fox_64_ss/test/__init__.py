from test.bases import WorldTestBase

from .. import StarFox64SSWorld


class StarFox64SSTestBase(WorldTestBase):
    game = "Star Fox 64 (Starship)"
    world: StarFox64SSWorld
