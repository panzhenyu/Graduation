if __name__ == "__main__":
	fd = open('mcf.txt', 'r')
	cache_size_idx = 0
	miss_num_idx = 2
	access_num_idx = 3

	raw_text = []
	for line in fd.readlines():
		raw_text.append(line.strip().split(' '))
	raw_data = []
	for row in raw_text:
		raw_data.append([int(x) for x in row])

	cache_size = []
	fetch_rate = []
	hit_rate = []

	for row in raw_data:
		cache_size.append(row[cache_size_idx])
		fetch, access = row[miss_num_idx], row[access_num_idx]
		hit = access - fetch
		fetch_rate.append(float(fetch) / access)
		hit_rate.append(float(hit) / access)

	print("cache_size:", cache_size)
	print("fetch_rate:", fetch_rate)
	print("hit_rate:", hit_rate)
	