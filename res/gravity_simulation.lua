-- GravitySimulation.lua
-- A physics simulation with gravitational bodies and interactive elements

GravitySim = {
    transform = nil,
    renderer = nil,
    bodies = {},
    trails = {},
    mouse_mass = 1000.0,
    gravitational_constant = 50.0,
    max_bodies = 30,
    max_trail_length = 50,
    time = 0,
    paused = false,
    show_forces = false
}

function Init()
    GravitySim.transform = gameObject:GetTransformComponent()
    GravitySim.renderer = get_renderer()
    
    -- Create some initial bodies
    spawn_random_bodies(8)
end

function Update(dt)
    GravitySim.time = GravitySim.time + dt
    
    -- Handle input
    handle_input(dt)
    
    if not GravitySim.paused then
        -- Update physics
        update_physics(dt)
        
        -- Update trails
        update_trails()
    end
    
    -- Clean up old bodies that went off screen
    cleanup_bodies()
end

function Render()
    -- Draw trails first (behind bodies)
    draw_trails()
    
    -- Draw gravitational bodies
    draw_bodies()
    
    -- Draw mouse influence
    draw_mouse_influence()
    
    -- Draw force vectors if enabled
    if GravitySim.show_forces then
        draw_force_vectors()
    end
    
    -- Draw UI indicators
    draw_ui()
end

function handle_input(dt)
    -- Left click: Add new body at mouse position
    if is_left_mouse_down() then
        local mouse_x, mouse_y = get_mouse_position()
        if #GravitySim.bodies < GravitySim.max_bodies then
            create_body(mouse_x, mouse_y)
        end
    end
    
    -- Right click: Attract bodies to mouse (like a black hole)
    -- Middle click: Repel bodies from mouse
    -- (These are handled in update_physics)
end

function update_physics(dt)
    local mouse_x, mouse_y = get_mouse_position()
    
    for i, body in ipairs(GravitySim.bodies) do
        local total_force = vec3(0, 0, 0)
        
        -- Calculate gravitational forces from other bodies
        for j, other_body in ipairs(GravitySim.bodies) do
            if i ~= j then
                local force = calculate_gravitational_force(body, other_body)
                total_force = total_force + force
            end
        end
        
        -- Mouse interaction forces
        local mouse_pos = vec3(mouse_x, mouse_y, 0)
        local to_mouse = mouse_pos - body.pos
        local distance_to_mouse = to_mouse:length()
        
        if distance_to_mouse > 1.0 then
            local mouse_force_magnitude = 0
            
            if is_right_mouse_down() then
                -- Attraction to mouse
                mouse_force_magnitude = (GravitySim.mouse_mass * body.mass) / (distance_to_mouse * distance_to_mouse) * GravitySim.gravitational_constant
                total_force = total_force + to_mouse:normalize() * mouse_force_magnitude
            elseif is_middle_mouse_down() then
                -- Repulsion from mouse
                mouse_force_magnitude = (GravitySim.mouse_mass * body.mass) / (distance_to_mouse * distance_to_mouse) * GravitySim.gravitational_constant
                total_force = total_force - to_mouse:normalize() * mouse_force_magnitude
            end
        end
        
        -- Apply forces (F = ma, so a = F/m)
        local acceleration = total_force / body.mass
        body.velocity = body.velocity + acceleration * dt
        body.pos = body.pos + body.velocity * dt
        
        -- Store force for visualization
        body.last_force = total_force
        
        -- Add position to trail
        table.insert(body.trail, {x = body.pos.x, y = body.pos.y, time = GravitySim.time})
        
        -- Limit trail length
        while #body.trail > GravitySim.max_trail_length do
            table.remove(body.trail, 1)
        end
    end
end

function calculate_gravitational_force(body1, body2)
    local direction = body2.pos - body1.pos
    local distance = direction:length()
    
    if distance < 1.0 then
        return vec3(0, 0, 0) -- Avoid division by zero and extreme forces
    end
    
    local force_magnitude = (GravitySim.gravitational_constant * body1.mass * body2.mass) / (distance * distance)
    return direction:normalize() * force_magnitude
end

function update_trails()
    -- Remove old trail points
    local current_time = GravitySim.time
    for i, body in ipairs(GravitySim.bodies) do
        for j = #body.trail, 1, -1 do
            if current_time - body.trail[j].time > 3.0 then -- Trail lasts 3 seconds
                table.remove(body.trail, j)
            end
        end
    end
end

function draw_bodies()
    for i, body in ipairs(GravitySim.bodies) do
        -- Color based on mass and velocity
        local speed = body.velocity:length()
        local r = math.min(1.0, body.mass / 100.0)
        local g = math.min(1.0, speed / 100.0)
        local b = 0.5 + math.sin(GravitySim.time + i) * 0.5
        
        GravitySim.renderer:set_draw_color_float(r, g, b, 0.8)
        
        -- Draw body as a filled circle
        local radius = math.sqrt(body.mass / math.pi) * 0.5 + 3 -- Size based on mass
        draw_filled_circle(body.pos, radius, 12)
        
        -- Draw velocity vector
        GravitySim.renderer:set_draw_color_float(1.0, 1.0, 1.0, 0.5)
        local vel_end = body.pos + body.velocity * 0.1
        GravitySim.renderer:render_line(body.pos.x, body.pos.y, vel_end.x, vel_end.y)
    end
end

function draw_trails()
    for i, body in ipairs(GravitySim.bodies) do
        if #body.trail > 1 then
            for j = 1, #body.trail - 1 do
                local point1 = body.trail[j]
                local point2 = body.trail[j + 1]
                
                local age = GravitySim.time - point1.time
                local alpha = math.max(0, 1 - age / 3.0) * 0.3
                
                GravitySim.renderer:set_draw_color_float(0.8, 0.8, 1.0, alpha)
                GravitySim.renderer:render_line(point1.x, point1.y, point2.x, point2.y)
            end
        end
    end
end

function draw_mouse_influence()
    local mouse_x, mouse_y = get_mouse_position()
    
    if is_right_mouse_down() then
        -- Draw attraction field
        GravitySim.renderer:set_draw_color_float(0.2, 1.0, 0.2, 0.3)
        for radius = 20, 100, 20 do
            draw_circle(vec3(mouse_x, mouse_y, 0), radius, 16)
        end
    elseif is_middle_mouse_down() then
        -- Draw repulsion field
        GravitySim.renderer:set_draw_color_float(1.0, 0.2, 0.2, 0.3)
        for radius = 20, 100, 20 do
            draw_circle(vec3(mouse_x, mouse_y, 0), radius, 16)
        end
    end
end

function draw_force_vectors()
    for i, body in ipairs(GravitySim.bodies) do
        if body.last_force then
            GravitySim.renderer:set_draw_color_float(1.0, 0.5, 0.0, 0.7)
            local force_end = body.pos + body.last_force * 0.01 -- Scale down for visibility
            GravitySim.renderer:render_line(body.pos.x, body.pos.y, force_end.x, force_end.y)
        end
    end
end

function draw_ui()
    local window = get_window()
    
    -- Draw simple indicators
    GravitySim.renderer:set_draw_color_float(1.0, 1.0, 1.0, 0.8)
    
    -- Body count indicator (simple bar)
    local bar_width = (#GravitySim.bodies / GravitySim.max_bodies) * 100
    GravitySim.renderer:render_line(10, 10, 10 + bar_width, 10)
    GravitySim.renderer:render_line(10, 11, 10 + bar_width, 11)
    
    -- Pause indicator
    if GravitySim.paused then
        GravitySim.renderer:render_line(window.width - 30, 10, window.width - 30, 20)
        GravitySim.renderer:render_line(window.width - 25, 10, window.width - 25, 20)
    end
end

function create_body(x, y)
    local body = {
        pos = vec3(x, y, 0),
        velocity = vec3((math.random() - 0.5) * 50, (math.random() - 0.5) * 50, 0),
        mass = 20 + math.random() * 80,
        trail = {},
        last_force = vec3(0, 0, 0)
    }
    table.insert(GravitySim.bodies, body)
end

function spawn_random_bodies(count)
    local window = get_window()
    for i = 1, count do
        local x = math.random() * window.width
        local y = math.random() * window.height
        create_body(x, y)
    end
end

function cleanup_bodies()
    local window = get_window()
    for i = #GravitySim.bodies, 1, -1 do
        local body = GravitySim.bodies[i]
        if body.pos.x < -100 or body.pos.x > window.width + 100 or
           body.pos.y < -100 or body.pos.y > window.height + 100 then
            table.remove(GravitySim.bodies, i)
        end
    end
end

function draw_circle(pos, radius, segments)
    for i = 0, segments - 1 do
        local angle1 = (i / segments) * 2 * math.pi
        local angle2 = ((i + 1) / segments) * 2 * math.pi
        
        local x1 = pos.x + math.cos(angle1) * radius
        local y1 = pos.y + math.sin(angle1) * radius
        local x2 = pos.x + math.cos(angle2) * radius
        local y2 = pos.y + math.sin(angle2) * radius
        
        GravitySim.renderer:render_line(x1, y1, x2, y2)
    end
end

function draw_filled_circle(pos, radius, segments)
    -- Draw multiple concentric circles for a "filled" effect
    for r = 2, radius, 2 do
        draw_circle(pos, r, segments)
    end
end