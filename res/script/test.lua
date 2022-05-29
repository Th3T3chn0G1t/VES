function update(entity, delta)
    amt = {x = 5.0 * delta, y = 5.0 * delta}
    ves.planar_move(entity, amt)
end

function red_teapot_update(entity, delta)
    amt = {x = math.random() - 0.5, y = math.random() - 0.5}
    ves.planar_move(entity, amt)
end

function select(entity, delta)
    print("Ayyy! " .. delta)
end

math.randomseed(os.time())

ves.register("update", update, "c")
ves.register("update", red_teapot_update, "a")

ves.register("select", select, "a")
