import sys
from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import letter

def draw_sudoku(size, top, left, sudoku, label, pdf):
    right = size + left
    bottom = top-size

    thinLine=1
    thickLine=4

    box_height = size / 9 

    #Draw the label
    fontSize = 12
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

    for row in range(0, 9):
        for col in range(0, 9):
            num = sudoku[row * 9 + col]
            if num != '0':
                pdf.drawString(left + col * box_height + box_height * 0.38, top - row * box_height - box_height * 0.65, num)


if __name__ == "__main__":
    #Parse the arguments
    if len(sys.argv) != 3:
        print "usage:",sys.argv[0],"input_file output_file"
        sys.exit(1)
        

    input_file = open(sys.argv[1],'r')
    output_file = sys.argv[2]
    pdf = canvas.Canvas(output_file)

    width, height = letter
    #Size of a sudoku
    size = width*6.0/16.0

    #The sudokus can be in the upper or lower part of the page
    upper_y = height*0.75 + size*0.5
    lower_y = height*0.25 + size*0.5
    #The sudokus can be centered, to the left or right
    center_x = width*0.5 - size*0.5
    left_x = width*0.25 - size*0.5
    right_x = width*0.75 - size*0.5

    #Read the number of test cases
    cases = int(input_file.readline())

    for case_num in range(1,cases+1):
        if case_num != 1:
            pdf.showPage()

        #Print case num
        pdf.setFont('Times-Bold',15)
        pdf.drawString(width*0.03,height*0.97,"Case #"+str(case_num))

        #Cardinality of the sudoku
        sudoku_card = int(input_file.readline())

        #Sudoku to solve (semi-empty instance)
        sudoku = (input_file.readline()).split()

        #The base sudoku is centred and in the top
        left = center_x
        top = upper_y

        draw_sudoku(size, top, left, sudoku, "Base Puzzle", pdf)

        #Now read the number of other solutions
        solutions = int(input_file.readline())

        for sol_num in range(solutions):
            #Calculate if it's a new page!
            if sol_num%4 == 2:
                pdf.showPage()

            #Read the name of the solver and the time it performed
            solver_name = input_file.readline().strip()
            sol_time_ms = input_file.readline().strip() + "ms"

            #Read the solution
            sudoku = input_file.readline().split()

            #Set the corners            
            left = [left_x,right_x][sol_num%2]
            top = [lower_y,lower_y,upper_y,upper_y][sol_num%4]

            label = solver_name + "'s solution ("+sol_time_ms+")"
            draw_sudoku(size, top, left, sudoku, label, pdf)

    pdf.save()
