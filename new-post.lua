
function Concat(...)
    local origin = {...}
    local message = ""
    for i,v in pairs(origin) do
       message = message .. v
    end
    return message
end

local date = os.date("%Y-%m-%d");
local time = os.date("%H:%M:%S");

local current_path = string.sub(debug.getinfo(1).source, 2, string.len("new-post.lua") * -1 - 1)
local post_path = Concat(current_path, "_posts/", date, "-post.md")


local file = io.open(post_path, "w+")

file:write("---\n")
file:write("layout: post\n")
file:write("title:  \n")
file:write("date:   ", date, " ", time, "\n")
file:write("categories: \n")
file:write("tags: \n")
file:write("---\n\n")
file:write("* content\n")
file:write("{:toc}\n\n")

file:close()

