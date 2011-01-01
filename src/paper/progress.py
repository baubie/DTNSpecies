import sys

def update(count,total,id=0,width=50):
    if count < 0 or count > total: return

    width = max([5,width]) # Minimum of 5 characters
    mid = int(width/2)-1
    percent = float(count)/total

    show = int(round(width*percent))

    left_bars = min(mid,show)
    left_fill = max(0,mid-left_bars)
    right_bars = max(0,show-mid)
    right_fill = max(0,width-mid-right_bars)

    sys.stdout.write("\r")
    sys.stdout.write(" "*(width+2))
    sys.stdout.write("\r")
    sys.stdout.write("[")

    sys.stdout.write("%s%s" % ("="*left_bars," "*left_fill))
    if (count < total):
        sys.stdout.write("%02d" % int(round(percent*100)))
        sys.stdout.write("%")
    else:
        sys.stdout.write("100")

    sys.stdout.write("%s%s" % ("="*right_bars," "*right_fill))

    sys.stdout.write("]")
    sys.stdout.flush()



