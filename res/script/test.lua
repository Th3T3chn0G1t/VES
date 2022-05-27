function update(entity)
    amt = ves.vec2.new()
    amt.x = 0.1
    amt.y = 0.1
    ves.planar_move(entity, amt)
end
