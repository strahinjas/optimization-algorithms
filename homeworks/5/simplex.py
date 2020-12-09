import matplotlib.pyplot as plt


def plotFromFile(fileName, xLabel, yLabel):
    file = open(fileName, 'r')

    x = []
    y = []

    for line in file:
        s = line.split()

        if len(s) != 0:
            x.append(float(s[0]))
            y.append(float(s[1]))

    file.close()

    plt.figure()
    plt.plot(x, y)
    plt.xlabel(xLabel)
    plt.ylabel(yLabel)
    plt.show()


plotFromFile('C:\\Users\\User\\Desktop\\Strahinja\\OOA\\Domaći\\5\\vbe.txt', 'E', 'Vbe')
plotFromFile('C:\\Users\\User\\Desktop\\Strahinja\\OOA\\Domaći\\5\\ic.txt', 'E', 'Ic')
plotFromFile('C:\\Users\\User\\Desktop\\Strahinja\\OOA\\Domaći\\5\\vout.txt', 'E', 'Vout')
