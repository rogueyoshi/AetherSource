SetResolution(GetDisplayWidth(), GetDisplayHeight(), 30);

local catImage = LoadImage('cat.png')

local cats = {}

for i = 1, 32 do
	local cat = {
		image = catImage,
		xPosition = math.random(GetWidth()),
		yPosition = math.random(GetHeight())
	}

	table.insert(cats, cat)
end

local font = LoadFont("Arial")

local time = 0

function OnDestroy()
	ReleaseImage(catImage)
	ReleaseFont(font)
end

function OnUpdate(deltaTime)
	print(Keyboard.A)

	for _, cat in pairs(cats) do
		--
	end
	
	time = time + deltaTime
end

function OnRender(deltaTime)	
	for _, cat in pairs(cats) do
		DrawSprite
		{
			image = cat.image,
			x = cat.xPosition + math.sin(time * 8) * 16,
			y = cat.yPosition + math.sin(time * 4) * 16
		}
	end
	
	DrawText
	{
		text = 'Hello, world!',
		font = font,
		size = 128,
		x = 100,
		y = 50,
		color = 0xFF0099FF
	}
end
