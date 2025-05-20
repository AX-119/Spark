local hasExecuted = false

function Update()
    if not hasExecuted then
        print("Hello from Lua!")
        hasExecuted = true
    end
end