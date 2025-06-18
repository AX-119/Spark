-- DraggablePhysics.lua
-- A physics simulation with draggable bodies, springs, and collisions

DragSim = {
    transform = nil,
    renderer = nil,
    bodies = {},
    springs = {},
    dragging = nil,
    drag_offset = vec3(0, 0, 0),
    mouse_pos = vec3(0, 0, 0),
    last_mouse_pos = vec3(0, 0, 0),
    time = 0,
    paused = false,
    show_connections = true,
    gravity = vec3(0, 100, 0), -- Downward gravity
    damping = 0.995,
    max_bodies = 25,
    collision_enabled = true
}

function Init()
    DragSim.transform = gameObject:GetTransformComponent()
    DragSim.renderer = get_renderer()
    
    -- Create some initial setup
    create_demo_scene()
end

function Update(dt)
    DragSim.time = DragSim.time + dt
    
    -- Store previous mouse position
    DragSim.last_mouse_pos = DragSim.mouse_pos
    local mouse_x, mouse_y = get_mouse_position()
    DragSim.mouse_pos = vec3(mouse_x, mouse_y, 0)
    
    -- Handle input
    handle_input(dt)
    
    if not DragSim.paused then
        -- Update physics
        update_physics(dt)
        
        -- Handle collisions
        if DragSim.collision_enabled then
            handle_collisions()
        end
    end
    
    -- Clean up bodies that fall too far
    cleanup_bodies()
end

function Render()
    -- Draw springs/connections first
    if DragSim.show_connections then
        draw_springs()
    end
    
    -- Draw bodies
    draw_bodies()
    
    -- Draw drag indicator
    draw_drag_indicator()
    
    -- Draw UI
    draw_ui()
end

function handle_input(dt)
    local mouse_x, mouse_y = get_mouse_position()
    
    -- Left mouse button handling
    if is_left_mouse_down() then
        if DragSim.dragging == nil then
            -- Start dragging - find body under mouse
            DragSim.dragging = find_body_at_position(mouse_x, mouse_y)
            if DragSim.dragging then
                DragSim.drag_offset = DragSim.dragging.pos - vec3(mouse_x, mouse_y, 0)
                -- Reduce velocity when grabbed
                DragSim.dragging.velocity = DragSim.dragging.velocity * 0.3
            end
        else
            -- Continue dragging
            local target_pos = vec3(mouse_x, mouse_y, 0) + DragSim.drag_offset
            local drag_force = (target_pos - DragSim.dragging.pos) * 800 -- Spring-like drag
            DragSim.dragging.velocity = DragSim.dragging.velocity + drag_force * dt / DragSim.dragging.mass
        end
    else
        -- Release drag
        if DragSim.dragging then
            -- Add mouse velocity to body when released
            local mouse_velocity = (DragSim.mouse_pos - DragSim.last_mouse_pos) / dt
            DragSim.dragging.velocity = DragSim.dragging.velocity + mouse_velocity * 0.5
            DragSim.dragging = nil
        end
    end
    
    -- Right click: Create new body
    if is_right_mouse_down() then
        if #DragSim.bodies < DragSim.max_bodies then
            create_body(mouse_x, mouse_y)
        end
    end
    
    -- Middle click: Create spring connection
    if is_middle_mouse_down() then
        local body = find_body_at_position(mouse_x, mouse_y)
        if body and DragSim.dragging and body ~= DragSim.dragging then
            create_spring(DragSim.dragging, body)
        end
    end
end

function update_physics(dt)
    -- Apply forces to all bodies
    for i, body in ipairs(DragSim.bodies) do
        -- Reset forces
        body.force = vec3(0, 0, 0)
        
        -- Apply gravity (skip if being dragged)
        if body ~= DragSim.dragging then
            body.force = body.force + DragSim.gravity * body.mass
        end
        
        -- Apply spring forces
        for j, spring in ipairs(DragSim.springs) do
            if spring.body1 == body then
                local spring_force = calculate_spring_force(spring, body, spring.body2)
                body.force = body.force + spring_force
            elseif spring.body2 == body then
                local spring_force = calculate_spring_force(spring, body, spring.body1)
                body.force = body.force + spring_force
            end
        end
    end
    
    -- Integrate motion
    for i, body in ipairs(DragSim.bodies) do
        if body ~= DragSim.dragging then
            -- Apply forces (F = ma, so a = F/m)
            local acceleration = body.force / body.mass
            body.velocity = body.velocity + acceleration * dt
            
            -- Apply damping
            body.velocity = body.velocity * DragSim.damping
            
            -- Update position
            body.pos = body.pos + body.velocity * dt
            
            -- Boundary constraints (bouncy walls)
            local window = get_window()
            local radius = body.radius
            
            if body.pos.x - radius < 0 then
                body.pos.x = radius
                body.velocity.x = -body.velocity.x * 0.7
            elseif body.pos.x + radius > window.width then
                body.pos.x = window.width - radius
                body.velocity.x = -body.velocity.x * 0.7
            end
            
            if body.pos.y - radius < 0 then
                body.pos.y = radius
                body.velocity.y = -body.velocity.y * 0.7
            elseif body.pos.y + radius > window.height then
                body.pos.y = window.height - radius
                body.velocity.y = -body.velocity.y * 0.7
            end
        end
    end
end

function calculate_spring_force(spring, body1, body2)
    local direction = body2.pos - body1.pos
    local current_length = direction:length()
    
    if current_length < 0.1 then
        return vec3(0, 0, 0)
    end
    
    local displacement = current_length - spring.rest_length
    local spring_force_magnitude = spring.stiffness * displacement
    
    -- Add damping based on relative velocity
    local relative_velocity = body2.velocity - body1.velocity
    local direction_norm = direction:normalize()
    local relative_velocity_along_direction = relative_velocity.x * direction_norm.x + relative_velocity.y * direction_norm.y
    local damping_force = direction_norm * relative_velocity_along_direction * spring.damping
    
    return direction:normalize() * spring_force_magnitude + damping_force
end

function handle_collisions()
    for i = 1, #DragSim.bodies - 1 do
        for j = i + 1, #DragSim.bodies do
            local body1 = DragSim.bodies[i]
            local body2 = DragSim.bodies[j]
            
            local distance = (body2.pos - body1.pos):length()
            local min_distance = body1.radius + body2.radius
            
            if distance < min_distance and distance > 0.1 then
                -- Collision detected - separate and apply impulse
                local overlap = min_distance - distance
                local direction = (body2.pos - body1.pos):normalize()
                
                -- Separate bodies
                local separation = direction * overlap * 0.5
                if body1 ~= DragSim.dragging then
                    body1.pos = body1.pos - separation
                end
                if body2 ~= DragSim.dragging then
                    body2.pos = body2.pos + separation
                end
                
                -- Apply collision impulse
                local relative_velocity = body2.velocity - body1.velocity
                local velocity_along_normal = relative_velocity.x * direction.x + relative_velocity.y * direction.y
                
                if velocity_along_normal > 0 then
                    -- Objects separating, don't resolve
                    goto continue
                end
                
                local restitution = 0.6 -- Bounciness
                local impulse_magnitude = -(1 + restitution) * velocity_along_normal
                impulse_magnitude = impulse_magnitude / (1/body1.mass + 1/body2.mass)
                
                local impulse = direction * impulse_magnitude
                
                if body1 ~= DragSim.dragging then
                    body1.velocity = body1.velocity - impulse / body1.mass
                end
                if body2 ~= DragSim.dragging then
                    body2.velocity = body2.velocity + impulse / body2.mass
                end
                
                ::continue::
            end
        end
    end
end

function find_body_at_position(x, y)
    local mouse_pos = vec3(x, y, 0)
    for i, body in ipairs(DragSim.bodies) do
        local distance = (body.pos - mouse_pos):length()
        if distance <= body.radius then
            return body
        end
    end
    return nil
end

function create_body(x, y)
    local body = {
        pos = vec3(x, y, 0),
        velocity = vec3((math.random() - 0.5) * 100, (math.random() - 0.5) * 50, 0),
        force = vec3(0, 0, 0),
        mass = 10 + math.random() * 40,
        radius = 8 + math.random() * 15,
        color = {
            r = 0.3 + math.random() * 0.7,
            g = 0.3 + math.random() * 0.7,
            b = 0.3 + math.random() * 0.7
        }
    }
    table.insert(DragSim.bodies, body)
    return body
end

function create_spring(body1, body2)
    -- Check if spring already exists
    for i, spring in ipairs(DragSim.springs) do
        if (spring.body1 == body1 and spring.body2 == body2) or
           (spring.body1 == body2 and spring.body2 == body1) then
            return -- Spring already exists
        end
    end
    
    local spring = {
        body1 = body1,
        body2 = body2,
        rest_length = (body2.pos - body1.pos):length(),
        stiffness = 200 + math.random() * 300,
        damping = 5 + math.random() * 10
    }
    table.insert(DragSim.springs, spring)
end

function create_demo_scene()
    local window = get_window()
    
    -- Create a few bodies in interesting positions
    local body1 = create_body(window.width * 0.3, window.height * 0.3)
    local body2 = create_body(window.width * 0.7, window.height * 0.3)
    local body3 = create_body(window.width * 0.5, window.height * 0.6)
    
    -- Connect them with springs
    create_spring(body1, body2)
    create_spring(body2, body3)
    create_spring(body3, body1)
    
    -- Add some free bodies
    for i = 1, 4 do
        create_body(math.random() * window.width, math.random() * window.height * 0.5)
    end
end

function draw_bodies()
    for i, body in ipairs(DragSim.bodies) do
        -- Highlight dragged body
        local alpha = 0.8
        if body == DragSim.dragging then
            alpha = 1.0
            -- Draw selection indicator
            DragSim.renderer:set_draw_color_float(1.0, 1.0, 1.0, 0.5)
            draw_circle(body.pos, body.radius + 3, 16)
        end
        
        -- Set body color
        DragSim.renderer:set_draw_color_float(body.color.r, body.color.g, body.color.b, alpha)
        
        -- Draw body
        draw_filled_circle(body.pos, body.radius, 12)
        
        -- Draw velocity vector for dragged body
        if body == DragSim.dragging then
            DragSim.renderer:set_draw_color_float(1.0, 1.0, 0.0, 0.7)
            local vel_end = body.pos + body.velocity * 0.1
            DragSim.renderer:render_line(body.pos.x, body.pos.y, vel_end.x, vel_end.y)
        end
    end
end

function draw_springs()
    for i, spring in ipairs(DragSim.springs) do
        local current_length = (spring.body2.pos - spring.body1.pos):length()
        local strain = math.abs(current_length - spring.rest_length) / spring.rest_length
        
        -- Color based on strain (blue = relaxed, red = stretched/compressed)
        local r = math.min(1.0, strain * 2)
        local g = 0.3
        local b = math.max(0.3, 1.0 - strain)
        
        DragSim.renderer:set_draw_color_float(r, g, b, 0.6)
        DragSim.renderer:render_line(
            spring.body1.pos.x, spring.body1.pos.y,
            spring.body2.pos.x, spring.body2.pos.y
        )
        
        -- Draw spring coils for visual effect
        draw_spring_coils(spring)
    end
end

function draw_spring_coils(spring)
    local start_pos = spring.body1.pos
    local end_pos = spring.body2.pos
    local direction = end_pos - start_pos
    local length = direction:length()
    
    if length < 10 then return end
    
    local normalized = direction:normalize()
    local perpendicular = vec3(-normalized.y, normalized.x, 0)
    
    local coil_count = math.floor(length / 20)
    local segment_length = length / (coil_count * 2)
    
    DragSim.renderer:set_draw_color_float(0.5, 0.5, 0.8, 0.4)
    
    for i = 0, coil_count * 2 - 1 do
        local t1 = i / (coil_count * 2)
        local t2 = (i + 1) / (coil_count * 2)
        
        local pos1 = start_pos + direction * t1
        local pos2 = start_pos + direction * t2
        
        if i % 2 == 0 then
            pos1 = pos1 + perpendicular * 3
        else
            pos1 = pos1 - perpendicular * 3
        end
        
        if (i + 1) % 2 == 0 then
            pos2 = pos2 + perpendicular * 3
        else
            pos2 = pos2 - perpendicular * 3
        end
        
        DragSim.renderer:render_line(pos1.x, pos1.y, pos2.x, pos2.y)
    end
end

function draw_drag_indicator()
    if DragSim.dragging then
        local mouse_x, mouse_y = get_mouse_position()
        local target_pos = vec3(mouse_x, mouse_y, 0) + DragSim.drag_offset
        
        -- Draw line from mouse to drag target
        DragSim.renderer:set_draw_color_float(1.0, 1.0, 0.0, 0.6)
        DragSim.renderer:render_line(
            mouse_x, mouse_y,
            target_pos.x, target_pos.y
        )
        
        -- Draw cross at target position
        local size = 5
        DragSim.renderer:render_line(
            target_pos.x - size, target_pos.y,
            target_pos.x + size, target_pos.y
        )
        DragSim.renderer:render_line(
            target_pos.x, target_pos.y - size,
            target_pos.x, target_pos.y + size
        )
    end
end

function draw_ui()
    local window = get_window()
    
    DragSim.renderer:set_draw_color_float(1.0, 1.0, 1.0, 0.8)
    
    -- Body count indicator
    local bar_width = (#DragSim.bodies / DragSim.max_bodies) * 100
    DragSim.renderer:render_line(10, 10, 10 + bar_width, 10)
    DragSim.renderer:render_line(10, 11, 10 + bar_width, 11)
    
    -- Spring count indicator
    local spring_bar_width = math.min(100, #DragSim.springs * 5)
    DragSim.renderer:set_draw_color_float(0.5, 0.8, 1.0, 0.8)
    DragSim.renderer:render_line(10, 15, 10 + spring_bar_width, 15)
    DragSim.renderer:render_line(10, 16, 10 + spring_bar_width, 16)
    
    -- Instructions (simple dots/lines)
    DragSim.renderer:set_draw_color_float(0.7, 0.7, 0.7, 0.6)
    -- Left click indicator (dot)
    draw_filled_circle(vec3(window.width - 80, 20, 0), 3, 6)
    -- Right click indicator (plus)
    DragSim.renderer:render_line(window.width - 60, 20, window.width - 50, 20)
    DragSim.renderer:render_line(window.width - 55, 15, window.width - 55, 25)
    -- Middle click indicator (line)
    DragSim.renderer:render_line(window.width - 40, 20, window.width - 20, 20)
    
    -- Pause indicator
    if DragSim.paused then
        DragSim.renderer:set_draw_color_float(1.0, 1.0, 1.0, 0.8)
        DragSim.renderer:render_line(window.width - 30, 10, window.width - 30, 20)
        DragSim.renderer:render_line(window.width - 25, 10, window.width - 25, 20)
    end
end

function cleanup_bodies()
    local window = get_window()
    for i = #DragSim.bodies, 1, -1 do
        local body = DragSim.bodies[i]
        if body.pos.y > window.height + 200 then -- Fell too far down
            -- Remove springs connected to this body
            for j = #DragSim.springs, 1, -1 do
                local spring = DragSim.springs[j]
                if spring.body1 == body or spring.body2 == body then
                    table.remove(DragSim.springs, j)
                end
            end
            
            -- Remove the body
            if DragSim.dragging == body then
                DragSim.dragging = nil
            end
            table.remove(DragSim.bodies, i)
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
        
        DragSim.renderer:render_line(x1, y1, x2, y2)
    end
end

function draw_filled_circle(pos, radius, segments)
    for r = 2, radius, 2 do
        draw_circle(pos, r, segments)
    end
end