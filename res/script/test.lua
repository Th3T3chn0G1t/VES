function update(entity, delta)
    amt = {x = 5.0 * delta, y = 5.0 * delta}
    ves.planar_move(entity, amt)
end

function red_teapot_update(entity, delta)
    amt = {x = math.random() - 0.5, y = math.random() - 0.5}
    ves.planar_move(entity, amt)
end

function follow_cursor(entity, delta)
	cursor_pos = ves.get_cursor_pos_planar()
	entity_pos = ves.get_entity_pos_planar(entity)

	speed = 5.0

	if(cursor_pos.x > entity_pos.x) then
		dx = speed
	else
		dx = -speed
	end

	if(cursor_pos.y > entity_pos.y) then
		dy = speed
	else
		dy = -speed
	end
	
	amt = {x = dx * delta, y = dy * delta}
    ves.planar_move(entity, amt)
end

function select(entity, delta)
	cursor_pos = ves.get_cursor_pos_planar()
	entity_pos = ves.get_entity_pos_planar(entity)
    print("Entity - x:" .. entity_pos.x .. " y:" .. entity_pos.y)
	print("Cursor - x:" .. cursor_pos.x .. " y:" .. cursor_pos.y)
end

function pathfind_e(entity, delta)
    speed = {x = 5.0 * delta, y = 5.0 * delta}
	ves.pathfind_planar(entity, ves.get_cursor_pos_planar(), speed)
end

math.randomseed(os.time())

ves.register("update", red_teapot_update, "a")
ves.register("select", select, "a")

ves.register("update", follow_cursor, "b")
ves.register("select", select, "b")

ves.register("update", update, "c")
ves.register("select", select, "c")

ves.register("select", select, "d")

ves.register("update", pathfind_e, "e")
ves.register("select", select, "e")

