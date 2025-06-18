-- WavePainter.lua
-- Creates beautiful wave patterns that follow your mouse movements

WavePainter = {
    transform = nil,
    renderer = nil,
    wave_points = {},
    mouse_trail = {},
    time = 0,
    wave_frequency = 3.0,
    wave_amplitude = 50.0,
    trail_length = 100,
    wave_speed = 2.0,
    color_cycle_speed = 1.0
}

function Init()
    WavePainter.transform = gameObject:GetTransformComponent()
    WavePainter.renderer = get_renderer()
    
    -- Initialize wave points across the screen
    local window = get_window()
    local width = window.width
    
    for x = 0, width, 5 do
        table.insert(WavePainter.wave_points, {
            x = x,
            base_y = window.height / 2,
            offset = x * 0.01 -- Phase offset for wave propagation
        })
    end
end

function Update(dt)
    WavePainter.time = WavePainter.time + dt
    
    -- Track mouse position
    local mouse_x, mouse_y = get_mouse_position()
    table.insert(WavePainter.mouse_trail, {x = mouse_x, y = mouse_y, time = WavePainter.time})
    
    -- Limit trail length
    while #WavePainter.mouse_trail > WavePainter.trail_length do
        table.remove(WavePainter.mouse_trail, 1)
    end
    
    -- Update wave points based on mouse influence
    for i, point in ipairs(WavePainter.wave_points) do
        local influence = 0
        
        -- Calculate influence from recent mouse positions
        for j, trail_point in ipairs(WavePainter.mouse_trail) do
            local distance = math.abs(point.x - trail_point.x)
            local time_decay = math.max(0, 1 - (WavePainter.time - trail_point.time) * 0.5)
            local spatial_influence = math.max(0, 1 - distance / 200.0)
            
            influence = influence + spatial_influence * time_decay * (trail_point.y - point.base_y) * 0.01
        end
        
        point.current_y = point.base_y + 
                         math.sin(WavePainter.time * WavePainter.wave_speed + point.offset) * WavePainter.wave_amplitude +
                         influence * 100
    end
    
    -- Adjust wave properties with mouse buttons
    if is_left_mouse_down() then
        WavePainter.wave_frequency = WavePainter.wave_frequency + dt * 2.0
    end
    
    if is_right_mouse_down() then
        WavePainter.wave_amplitude = WavePainter.wave_amplitude + dt * 20.0
    end
    
    if is_middle_mouse_down() then
        -- Reset to defaults
        WavePainter.wave_frequency = 3.0
        WavePainter.wave_amplitude = 50.0
    end
end

function Render()
    -- Draw the main wave
    draw_wave()
    
    -- Draw mouse trail
    draw_mouse_trail()
    
    -- Draw interference patterns
    if #WavePainter.mouse_trail > 0 then
        draw_interference_patterns()
    end
    
    -- Draw instructions
    draw_instructions()
end

function draw_wave()
    if #WavePainter.wave_points < 2 then return end
    
    for i = 1, #WavePainter.wave_points - 1 do
        local point1 = WavePainter.wave_points[i]
        local point2 = WavePainter.wave_points[i + 1]
        
        -- Color based on wave height and time
        local color_offset = point1.current_y * 0.01 + WavePainter.time * WavePainter.color_cycle_speed
        local r = (math.sin(color_offset) + 1) * 0.5
        local g = (math.sin(color_offset + 2.1) + 1) * 0.5
        local b = (math.sin(color_offset + 4.2) + 1) * 0.5
        
        WavePainter.renderer:set_draw_color_float(r, g, b, 0.8)
        WavePainter.renderer:render_line(point1.x, point1.current_y, point2.x, point2.current_y)
        
        -- Draw a secondary wave with phase offset
        local secondary_y1 = point1.base_y + math.sin(WavePainter.time * WavePainter.wave_speed + point1.offset + math.pi) * WavePainter.wave_amplitude * 0.5
        local secondary_y2 = point2.base_y + math.sin(WavePainter.time * WavePainter.wave_speed + point2.offset + math.pi) * WavePainter.wave_amplitude * 0.5
        
        WavePainter.renderer:set_draw_color_float(r * 0.5, g * 0.5, b * 0.5, 0.4)
        WavePainter.renderer:render_line(point1.x, secondary_y1, point2.x, secondary_y2)
    end
end

function draw_mouse_trail()
    if #WavePainter.mouse_trail < 2 then return end
    
    for i = 1, #WavePainter.mouse_trail - 1 do
        local point1 = WavePainter.mouse_trail[i]
        local point2 = WavePainter.mouse_trail[i + 1]
        
        local age1 = WavePainter.time - point1.time
        local age2 = WavePainter.time - point2.time
        local alpha1 = math.max(0, 1 - age1 * 2.0)
        local alpha2 = math.max(0, 1 - age2 * 2.0)
        
        if alpha1 > 0 and alpha2 > 0 then
            WavePainter.renderer:set_draw_color_float(1.0, 1.0, 1.0, (alpha1 + alpha2) * 0.5)
            WavePainter.renderer:render_line(point1.x, point1.y, point2.x, point2.y)
        end
    end
end

function draw_interference_patterns()
    local mouse_x, mouse_y = get_mouse_position()
    
    -- Draw ripples emanating from mouse position
    for radius = 10, 200, 20 do
        local segments = math.floor(radius / 3)
        local alpha = math.max(0, 1 - radius / 200.0) * 0.3
        
        -- Animate the ripples
        local animated_radius = radius + math.sin(WavePainter.time * 5.0 - radius * 0.1) * 5.0
        
        WavePainter.renderer:set_draw_color_float(0.5, 0.8, 1.0, alpha)
        
        for i = 0, segments - 1 do
            local angle1 = (i / segments) * 2 * math.pi
            local angle2 = ((i + 1) / segments) * 2 * math.pi
            
            local x1 = mouse_x + math.cos(angle1) * animated_radius
            local y1 = mouse_y + math.sin(angle1) * animated_radius
            local x2 = mouse_x + math.cos(angle2) * animated_radius
            local y2 = mouse_y + math.sin(angle2) * animated_radius
            
            WavePainter.renderer:render_line(x1, y1, x2, y2)
        end
    end
end

function draw_instructions()
    -- Simple text-like instructions using lines (since we don't have text rendering)
    WavePainter.renderer:set_draw_color_float(1.0, 1.0, 1.0, 0.6)
    
    -- Draw some simple shapes to represent instructions
    -- Left click indicator (arrow up)
    WavePainter.renderer:render_line(50, 50, 40, 60)
    WavePainter.renderer:render_line(50, 50, 60, 60)
    WavePainter.renderer:render_line(50, 50, 50, 70)
    
    -- Right click indicator (arrow down)  
    WavePainter.renderer:render_line(100, 70, 90, 60)
    WavePainter.renderer:render_line(100, 70, 110, 60)
    WavePainter.renderer:render_line(100, 70, 100, 50)
    
    -- Middle click indicator (circle)
    for i = 0, 7 do
        local angle1 = (i / 8) * 2 * math.pi
        local angle2 = ((i + 1) / 8) * 2 * math.pi
        local x1 = 150 + math.cos(angle1) * 8
        local y1 = 60 + math.sin(angle1) * 8
        local x2 = 150 + math.cos(angle2) * 8
        local y2 = 60 + math.sin(angle2) * 8
        WavePainter.renderer:render_line(x1, y1, x2, y2)
    end
end