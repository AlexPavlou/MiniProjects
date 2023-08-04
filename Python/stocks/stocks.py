import requests
from bs4 import BeautifulSoup

# Returns Country, excahnge, price currency, price and price change in % of desired stock. The program does this by scraping said metadata from www.marketwatch.com/investing/stock/$stock

stock = input("Enter a stock codename: ")
url = 'https://www.marketwatch.com/investing/stock/' + stock
response = requests.get(url)
soup = BeautifulSoup(response.text,features="html.parser")

metas = soup.find_all('meta')

title = soup.find('meta',property="og:title")["content"]
print(title)
print("Country: " + str([ meta.attrs['content'] for meta in metas if 'name' in meta.attrs and meta.attrs['name'] == 'exchangeCountry' ]).strip("'[]''"))
print("exchange: " + str([ meta.attrs['content'] for meta in metas if 'name' in meta.attrs and meta.attrs['name'] == 'exchange' ]).strip("'[]''"))
print("Price Currency: " + str([ meta.attrs['content'] for meta in metas if 'name' in meta.attrs and meta.attrs['name'] == 'priceCurrency' ]).strip("'[]''"))
print("Price: " + str([ meta.attrs['content'] for meta in metas if 'name' in meta.attrs and meta.attrs['name'] == 'price' ]).strip("'[]''"))
print("Price Change%: " + str([ meta.attrs['content'] for meta in metas if 'name' in meta.attrs and meta.attrs['name'] == 'priceChangePercent' ]).strip("'[]''"))
