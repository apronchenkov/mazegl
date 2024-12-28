directions = {
    {1, 0},
    {0, 1},
    {-1, 0},
    {0, -1}
}

local Tracer = {}

function Tracer:new()
    local instance = {
        x = 0,
        y = 0,
        d = 1,
        entrance_x = 0,
        entrance_y = 0,
        exit_x = 1,
        exit_y = 0,
        stack = {},
        map = {}
    }
    setmetatable(instance, self)
    self.__index = self
    return instance
end

function Tracer:_set_hall()
    local row = self.map[self.y]
    if row == nil then
        row = {}
        self.map[self.y] = row
    end
    row[self.x] = true
end

function Tracer:_get(x, y)
    if x == self.entrance_x and y == self.entrance_y then
        return "+"
    elseif x == self.exit_x and y == self.exit_y then
        return "-"
    end
    local row = self.map[y]
    if row ~= nil then
        if row[x] ~= nil then
            return "."
        end
    end
    return "#"
end

function Tracer:_span(x, y)
    local min_x, max_x, min_y, max_y = 0, 0, 0, 0
    for y, row in pairs(self.map) do
        if y < min_y then
            min_y = y
        elseif y > max_y then
            max_y = y
        end
        for x in pairs(row) do
            if x < min_x then
                min_x = x
            elseif x > max_x then
                max_x = x
            end
        end
    end
    return min_x, max_x, min_y, max_y
end

function Tracer:_render()
    local min_x, max_x, min_y, max_y = self:_span()
    local result = {}
    for y = max_y, min_y, -1 do
        for x = min_x, max_x do
            result[#result + 1] = self:_get(x, y)
        end
        result[#result + 1] = "\n"
    end
    return table.concat(result)
end

function Tracer:push()
    self.stack[#self.stack + 1] = {self.x, self.y, self.d}
end

function Tracer:pop()
    self.x, self.y, self.d = table.unpack(table.remove(self.stack))
end

function Tracer:go(n)
    for i = 1, n do
        self.x = self.x + directions[self.d][1]
        self.y = self.y + directions[self.d][2]
        self:_set_hall()
    end
end

function Tracer:turn_left()
    self.d = 1 + math.fmod(self.d, 4) % 4
end

function Tracer:turn_right()
    self.d = 1 + math.fmod(2 + self.d, 4)
end

function Tracer:set_entrance()
    self.entrance_x = self.x
    self.entrance_y = self.y
end

function Tracer:set_exit()
    self.exit_x = self.x
    self.exit_y = self.y
end


local P = { Tracer = Tracer }
if _REQUIREDNAME == nil then
  tracer = P
else
  _G[_REQUIREDNAME] = P
end
