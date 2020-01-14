from src.Curve import *

if __name__ == "__main__":
	hit = HitRate()
	sticky = StickyCache()
	hit.x = [0, 1, 2, 3]
	hit.y = [0, 0.5, 0.5, 1]
	hit.point_num = 4
	sticky.load(hit)
	print(sticky.y)
	print(sticky.getY(3))
	print(sticky.getX(3-2))
