#include "maptile.h"

MapTile::MapTile() noexcept : data(MTType::BLANK) {}
MapTile::MapTile(TileData data) noexcept : data(data) {}

TileData MapTile::val() const noexcept {
    return data;
}

void MapTile::set_val(TileData data) noexcept {
    this->data = data;
}

bool MapTile::has(MTType type) const noexcept {
    return (data & type) == type;
}

void MapTile::set(MTType type) noexcept {
    data = type;
}

void MapTile::append(MTType type) noexcept {
    data |= type;
}

void MapTile::remove(MTType type) noexcept {
    data &= ~type;
}

MapTile& MapTile::operator+=(MTType type) noexcept {
    append(type);
    return *this;
}

MapTile& MapTile::operator-=(MTType type) noexcept {
    remove(type);
    return *this;
}

bool MapTile::operator==(const MapTile& rhs) const noexcept {
    return data == rhs.data;
}
