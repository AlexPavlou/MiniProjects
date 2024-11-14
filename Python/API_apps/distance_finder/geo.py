from geopy.geocoders import Nominatim
from haversine import haversine, Unit

# Calling Nominatim
loc = Nominatim(user_agent="GetLoc")

loc1 = input("First location: ")
loc2 = input("Second location: ")
getLoc = loc.geocode(loc1)
getLoc2 = loc.geocode(loc2)

fir = (getLoc.latitude, getLoc.longitude)
sec = (getLoc2.latitude, getLoc2.longitude)
print(
    "The distance between "
    + loc1
    + " and "
    + loc2
    + " is: "
    + str(haversine(fir, sec, unit=Unit.KILOMETERS))
)
