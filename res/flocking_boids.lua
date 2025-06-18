-- FlockingBoids.lua
-- A flocking simulation with separation, alignment, and cohesion behaviors

FlockingSim = {
    transform = nil,
    renderer = nil,
    boids = {},
    predators = {},
    max_boids = 60,
    max_predators = 3,
    time = 0,
    
    -- Flocking parameters
    separation_radius = 25.0,
    alignment_radius = 50.0,
    cohesion_radius = 50.0,
    max_speed = 80.0,
    max_force = 100.0,
    
    -- Behavior weights
    separation_weight = 2.0,
    alignment_weight = 1.0,
    cohesion_weight = 1.0,
    avoidance_weight = 3.0,
    
    show_debug = false
}

function Init()
    FlockingSim.transform = gameObject:GetTransformComponent()
    FlockingSim.renderer = get_renderer()
    
    -- Create initial flock
    spawn_boids(40)
    spawn_predator()
end

function Update(dt)
    FlockingSim.time = FlockingSim.time + dt
    
    -- Handle input
    handle_input(dt)
    
    -- Update boids
    update_boids(dt)
    
    -- Update predators
    update_predators(dt)
    
    -- Keep entities in bounds
    wrap_entities()
end

function Render()
    get_renderer():set_draw_color(2, 13, 48, 255)
    -- Draw boids
    draw_boids()
    
    -- Draw predators
    draw_predators()
    
    -- Draw debug info
    if FlockingSim.show_debug then
        draw_debug_info()
    end
    
    -- Draw mouse influence
    draw_mouse_influence()
    
    -- Draw UI
    draw_ui()
end

function handle_input(dt)
    -- Left click: Add boid at mouse position
    if is_left_mouse_down() then
        local mouse_x, mouse_y = get_mouse_position()
        if #FlockingSim.boids < FlockingSim.max_boids then
            create_boid(mouse_x, mouse_y)
        end
    end
    
    -- Right click: Add predator
    if is_right_mouse_down() then
        local mouse_x, mouse_y = get_mouse_position()
        if #FlockingSim.predators < FlockingSim.max_predators then
            create_predator(mouse_x, mouse_y)
        end
    end
end

function create_boid(x, y)
    local boid = {
        pos = vec3(x, y, 0),
        velocity = vec3((math.random() - 0.5) * 60, (math.random() - 0.5) * 60, 0),
        acceleration = vec3(0, 0, 0),
        size = 3 + math.random() * 2,
        color_offset = math.random() * 2 * math.pi,
        fear_level = 0.0 -- How scared the boid is
    }
    table.insert(FlockingSim.boids, boid)
end

function create_predator(x, y)
    local predator = {
        pos = vec3(x, y, 0),
        velocity = vec3((math.random() - 0.5) * 40, (math.random() - 0.5) * 40, 0),
        size = 6 + math.random() * 3,
        hunt_timer = 0.0,
        target = nil
    }
    table.insert(FlockingSim.predators, predator)
end

function spawn_boids(count)
    local window = get_window()
    for i = 1, count do
        local x = math.random() * window.width
        local y = math.random() * window.height
        create_boid(x, y)
    end
end

function spawn_predator()
    local window = get_window()
    local x = math.random() * window.width
    local y = math.random() * window.height
    create_predator(x, y)
end

function update_boids(dt)
    local mouse_x, mouse_y = get_mouse_position()
    local mouse_pos = vec3(mouse_x, mouse_y, 0)
    
    for i, boid in ipairs(FlockingSim.boids) do
        -- Reset acceleration
        boid.acceleration = vec3(0, 0, 0)
        
        -- Calculate flocking forces
        local sep = separate(boid)
        local ali = align(boid)
        local coh = cohesion(boid)
        local avoid = avoid_predators(boid)
        
        -- Apply weights
        sep = sep * FlockingSim.separation_weight
        ali = ali * FlockingSim.alignment_weight
        coh = coh * FlockingSim.cohesion_weight
        avoid = avoid * FlockingSim.avoidance_weight
        
        -- Mouse interaction (middle mouse repels boids)
        if is_middle_mouse_down() then
            local distance_to_mouse = (boid.pos - mouse_pos):length()
            if distance_to_mouse < 100 and distance_to_mouse > 1 then
                local flee_force = (boid.pos - mouse_pos):normalize() * (100 / distance_to_mouse) * 2
                boid.acceleration = boid.acceleration + flee_force
            end
        end
        
        -- Combine all forces
        boid.acceleration = boid.acceleration + sep + ali + coh + avoid
        
        -- Limit force
        if boid.acceleration:length() > FlockingSim.max_force then
            boid.acceleration = boid.acceleration:normalize() * FlockingSim.max_force
        end
        
        -- Update velocity and position
        boid.velocity = boid.velocity + boid.acceleration * dt
        
        -- Speed adjustment based on fear
        local speed_multiplier = 1.0 + boid.fear_level * 0.5
        local current_max_speed = FlockingSim.max_speed * speed_multiplier
        
        if boid.velocity:length() > current_max_speed then
            boid.velocity = boid.velocity:normalize() * current_max_speed
        end
        
        boid.pos = boid.pos + boid.velocity * dt
        
        -- Decay fear over time
        boid.fear_level = math.max(0, boid.fear_level - dt * 2.0)
    end
end

function separate(boid)
    local desired_separation = FlockingSim.separation_radius
    local steer = vec3(0, 0, 0)
    local count = 0
    
    for i, other in ipairs(FlockingSim.boids) do
        if other ~= boid then
            local distance = (boid.pos - other.pos):length()
            if distance > 0 and distance < desired_separation then
                local diff = (boid.pos - other.pos):normalize()
                diff = diff / distance -- Weight by distance
                steer = steer + diff
                count = count + 1
            end
        end
    end
    
    if count > 0 then
        steer = steer / count
        steer = steer:normalize() * FlockingSim.max_speed
        steer = steer - boid.velocity
    end
    
    return steer
end

function align(boid)
    local neighbor_dist = FlockingSim.alignment_radius
    local sum = vec3(0, 0, 0)
    local count = 0
    
    for i, other in ipairs(FlockingSim.boids) do
        if other ~= boid then
            local distance = (boid.pos - other.pos):length()
            if distance > 0 and distance < neighbor_dist then
                sum = sum + other.velocity
                count = count + 1
            end
        end
    end
    
    if count > 0 then
        sum = sum / count
        sum = sum:normalize() * FlockingSim.max_speed
        local steer = sum - boid.velocity
        return steer
    else
        return vec3(0, 0, 0)
    end
end

function cohesion(boid)
    local neighbor_dist = FlockingSim.cohesion_radius
    local sum = vec3(0, 0, 0)
    local count = 0
    
    for i, other in ipairs(FlockingSim.boids) do
        if other ~= boid then
            local distance = (boid.pos - other.pos):length()
            if distance > 0 and distance < neighbor_dist then
                sum = sum + other.pos
                count = count + 1
            end
        end
    end
    
    if count > 0 then
        sum = sum / count
        return seek(boid, sum)
    else
        return vec3(0, 0, 0)
    end
end

function seek(boid, target)
    local desired = target - boid.pos
    desired = desired:normalize() * FlockingSim.max_speed
    local steer = desired - boid.velocity
    return steer
end

function avoid_predators(boid)
    local avoidance_radius = 80.0
    local steer = vec3(0, 0, 0)
    
    for i, predator in ipairs(FlockingSim.predators) do
        local distance = (boid.pos - predator.pos):length()
        if distance > 0 and distance < avoidance_radius then
            local flee_force = (boid.pos - predator.pos):normalize()
            flee_force = flee_force * (avoidance_radius / distance) -- Stronger when closer
            steer = steer + flee_force
            
            -- Increase fear level
            boid.fear_level = math.min(1.0, boid.fear_level + (1.0 / distance) * 10.0)
        end
    end
    
    return steer
end

function update_predators(dt)
    for i, predator in ipairs(FlockingSim.predators) do
        predator.hunt_timer = predator.hunt_timer + dt
        
        -- Find closest boid to hunt
        local closest_boid = nil
        local closest_distance = math.huge
        
        for j, boid in ipairs(FlockingSim.boids) do
            local distance = (predator.pos - boid.pos):length()
            if distance < closest_distance then
                closest_distance = distance
                closest_boid = boid
            end
        end
        
        if closest_boid then
            -- Seek behavior towards closest boid
            local desired = closest_boid.pos - predator.pos
            desired = desired:normalize() * (FlockingSim.max_speed * 0.7) -- Slightly slower than boids
            
            local steer = desired - predator.velocity
            predator.velocity = predator.velocity + steer * dt * 2.0
            
            -- Limit speed
            if predator.velocity:length() > FlockingSim.max_speed * 0.7 then
                predator.velocity = predator.velocity:normalize() * FlockingSim.max_speed * 0.7
            end
        end
        
        predator.pos = predator.pos + predator.velocity * dt
        
        -- Hunt success (remove boid if very close)
        if closest_boid and closest_distance < 8.0 and predator.hunt_timer > 0.5 then
            -- Remove the caught boid
            for j, boid in ipairs(FlockingSim.boids) do
                if boid == closest_boid then
                    table.remove(FlockingSim.boids, j)
                    predator.hunt_timer = 0.0
                    break
                end
            end
        end
    end
end

function wrap_entities()
    local window = get_window()
    local margin = 20
    
    -- Wrap boids
    for i, boid in ipairs(FlockingSim.boids) do
        if boid.pos.x < -margin then boid.pos.x = window.width + margin end
        if boid.pos.x > window.width + margin then boid.pos.x = -margin end
        if boid.pos.y < -margin then boid.pos.y = window.height + margin end
        if boid.pos.y > window.height + margin then boid.pos.y = -margin end
    end
    
    -- Wrap predators
    for i, predator in ipairs(FlockingSim.predators) do
        if predator.pos.x < -margin then predator.pos.x = window.width + margin end
        if predator.pos.x > window.width + margin then predator.pos.x = -margin end
        if predator.pos.y < -margin then predator.pos.y = window.height + margin end
        if predator.pos.y > window.height + margin then predator.pos.y = -margin end
    end
end

function draw_boids()
    for i, boid in ipairs(FlockingSim.boids) do
        -- Color based on fear level and velocity
        local speed = boid.velocity:length()
        local fear_color = boid.fear_level
        
        local r = 0.2 + fear_color * 0.8 + math.sin(FlockingSim.time + boid.color_offset) * 0.2
        local g = 0.5 + speed / FlockingSim.max_speed * 0.5 - fear_color * 0.3
        local b = 0.8 - fear_color * 0.5
        
        FlockingSim.renderer:set_draw_color_float(r, g, b, 0.8)
        
        -- Draw boid as triangle pointing in velocity direction
        draw_boid_triangle(boid)
        
        -- Draw velocity vector
        if FlockingSim.show_debug then
            FlockingSim.renderer:set_draw_color_float(1.0, 1.0, 1.0, 0.3)
            local vel_end = boid.pos + boid.velocity * 0.2
            FlockingSim.renderer:render_line(boid.pos.x, boid.pos.y, vel_end.x, vel_end.y)
        end
    end
end

function draw_boid_triangle(boid)
    local velocity = boid.velocity
    if velocity:length() < 1 then
        velocity = vec3(1, 0, 0) -- Default direction
    end
    
    local forward = velocity:normalize()
    local right = vec3(-forward.y, forward.x, 0)
    
    local size = boid.size
    local front = boid.pos + forward * size
    local back_left = boid.pos - forward * size * 0.5 + right * size * 0.5
    local back_right = boid.pos - forward * size * 0.5 - right * size * 0.5
    
    -- Draw triangle
    FlockingSim.renderer:render_line(front.x, front.y, back_left.x, back_left.y)
    FlockingSim.renderer:render_line(back_left.x, back_left.y, back_right.x, back_right.y)
    FlockingSim.renderer:render_line(back_right.x, back_right.y, front.x, front.y)
end

function draw_predators()
    for i, predator in ipairs(FlockingSim.predators) do
        -- Predator color with pulsing effect
        local pulse = 0.7 + math.sin(FlockingSim.time * 3 + i) * 0.3
        FlockingSim.renderer:set_draw_color_float(1.0 * pulse, 0.2, 0.2, 0.9)
        
        -- Draw predator as larger filled circle
        draw_filled_circle(predator.pos, predator.size, 8)
        
        -- Draw hunting radius when debug is on
        if FlockingSim.show_debug then
            FlockingSim.renderer:set_draw_color_float(1.0, 0.2, 0.2, 0.2)
            draw_circle(predator.pos, 80, 16)
        end
        
        -- Draw velocity vector
        FlockingSim.renderer:set_draw_color_float(1.0, 0.5, 0.5, 0.6)
        local vel_end = predator.pos + predator.velocity * 0.3
        FlockingSim.renderer:render_line(predator.pos.x, predator.pos.y, vel_end.x, vel_end.y)
    end
end

function draw_debug_info()
    -- Draw neighbor connections for first few boids
    for i = 1, math.min(3, #FlockingSim.boids) do
        local boid = FlockingSim.boids[i]
        
        -- Draw separation radius
        FlockingSim.renderer:set_draw_color_float(1.0, 0.0, 0.0, 0.2)
        draw_circle(boid.pos, FlockingSim.separation_radius, 12)
        
        -- Draw alignment/cohesion radius
        FlockingSim.renderer:set_draw_color_float(0.0, 1.0, 0.0, 0.1)
        draw_circle(boid.pos, FlockingSim.alignment_radius, 16)
        
        -- Draw connections to neighbors
        FlockingSim.renderer:set_draw_color_float(0.5, 0.5, 1.0, 0.3)
        for j, other in ipairs(FlockingSim.boids) do
            if other ~= boid then
                local distance = (boid.pos - other.pos):length()
                if distance < FlockingSim.alignment_radius then
                    FlockingSim.renderer:render_line(boid.pos.x, boid.pos.y, other.pos.x, other.pos.y)
                end
            end
        end
    end
end

function draw_mouse_influence()
    local mouse_x, mouse_y = get_mouse_position()
    
    if is_middle_mouse_down() then
        FlockingSim.renderer:set_draw_color_float(1.0, 1.0, 0.2, 0.4)
        for radius = 25, 100, 25 do
            draw_circle(vec3(mouse_x, mouse_y, 0), radius, 12)
        end
    end
end

function draw_ui()
    local window = get_window()
    
    FlockingSim.renderer:set_draw_color_float(1.0, 1.0, 1.0, 0.8)
    
    -- Boid count bar
    local boid_ratio = #FlockingSim.boids / FlockingSim.max_boids
    local bar_width = boid_ratio * 120
    for i = 0, 2 do
        FlockingSim.renderer:render_line(10, 10 + i, 10 + bar_width, 10 + i)
    end
    
    -- Predator indicators
    for i = 1, #FlockingSim.predators do
        FlockingSim.renderer:set_draw_color_float(1.0, 0.2, 0.2, 0.8)
        draw_filled_circle(vec3(10 + i * 15, 25, 0), 4, 6)
    end
    
    -- Debug toggle indicator
    if FlockingSim.show_debug then
        FlockingSim.renderer:set_draw_color_float(0.2, 1.0, 0.2, 0.8)
        FlockingSim.renderer:render_line(window.width - 20, 10, window.width - 10, 10)
        FlockingSim.renderer:render_line(window.width - 20, 15, window.width - 10, 15)
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
        
        FlockingSim.renderer:render_line(x1, y1, x2, y2)
    end
end

function draw_filled_circle(pos, radius, segments)
    for r = 1, radius, 1 do
        draw_circle(pos, r, segments)
    end
end