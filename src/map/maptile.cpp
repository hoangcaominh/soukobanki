#include "maptile.h"

MapTile::MapTile() noexcept : data(0) {}

TileData MapTile::val() const noexcept {
    return data;
}

bool MapTile::has(const Type &type) const noexcept {
    return data & type;
}

void MapTile::set(const Type &type) noexcept {
    data = type;
}

void MapTile::append(const Type &type) noexcept {
    data |= type;
}

void MapTile::remove(const Type &type) noexcept {
    data &= ~type;
}

MapTile& MapTile::operator+=(const Type &type) noexcept {
    append(type);
    return *this;
}

MapTile& MapTile::operator-=(const Type &type) noexcept {
    remove(type);
    return *this;
}
