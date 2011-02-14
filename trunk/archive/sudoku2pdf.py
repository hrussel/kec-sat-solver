import sys
from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import letter

def draw_sudoku(size, top, left, nums, label, pdf):
    right = size + left
    bottom = top-size

    thinLine=1
    thickLine=4

    box_height = size / 9 

    #Draw the label
    fontSize = 18
    pdf.setFont('Times-Bold',fontSize)
    pdf.drawString(left+box_height*0.5, top+box_height*0.5, label)

    #Draw the grid
    for i in range(0, 10):
        if i in [0, 3, 6, 9]: 
            pdf.setLineWidth(thickLine)
        else:
            pdf.setLineWidth(thinLine)
        pdf.line(left, top - i * box_height, right, top - i * box_height)
        pdf.line(left + i * box_height, top, left + i * box_height, bottom)

    #Draw numbers
    fontSize = 24

    nums = [str((x%9)+1) for x in range(3**4) ]

    for row in range(0, 9):
        for col in range(0, 9):
            num = nums[row * 9 + col]
            if num != '0':
                pdf.drawString(left + col * box_height + box_height * 0.38, top - row * box_height - box_height * 0.65, num)


if __name__ == "__main__":
    #Parse the arguments
    if len(sys.argv) != 2:
        print "You must specify an output filename (and only one)"
        sys.exit(1)
        

    filename = sys.argv[1]
    pdf = canvas.Canvas(filename)

    width, height = letter

    size = height*6.0/16.0
    left = (width - size)/2.0
    top1 = height*15.0/16.0
    top2 = height*7.0/16.0

    nums = [str((x%9)+1) for x in range(3**4) ]

    draw_sudoku(size, top1, left, nums, "Puzzle One", pdf)
    draw_sudoku(size, top2, left, nums, "Puzzle Two", pdf)

    pdf.save()
