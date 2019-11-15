import requests

URL = "http://crohmi.seecs.nust.edu.pk/datauploadscript.php"

dat = 2019
lat = 33.2
lng = 22.3
airm = 42.00
airt = 35.00
soilm1 = 0.00
soilt1 = 23.10
pn1 = 1

PARAMS = {'dat' : dat, 'lat' : lat, 'lng' : lng, 'airm' : airm, 'airt' : airt, 'soilm1' : soilm1, 'soilt1' : soilt1, 'pn1' : pn1}

r = requests.get(url = URL, params = PARAMS)

print("Done!")


