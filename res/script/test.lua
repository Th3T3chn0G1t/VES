function update(entity, delta)
    amt = {x = 5.0 * delta, y = 5.0 * delta}
    ves.planar_move(entity, amt)
end

function select(entity, delta)
    print("Ayyy! " .. delta)
end

ves.register("update", update, "c")
ves.register("select", select, "a")
