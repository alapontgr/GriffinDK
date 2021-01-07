TestProjects = 
	{
		"QuickTesting",
	}

group("Tests")
for k, Name in pairs(TestProjects) do
	include("./" .. Name)
end