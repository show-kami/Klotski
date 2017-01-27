library(ggplot2)
library(RColorBrewer)
library(animation)
library(tcltk)

b <- read.csv("result.csv")
last <- nrow(b)

pb <- txtProgressBar(min = 1, max = last, style = 3) # プログレスバーの設定
# プログレスバーについて http://gg-hogehoge.hatenablog.com/entry/2013/08/18/095448

x <- c(0,1,2,3,4)
y <- c(0,0,0,0,5)
empty <- data.frame(x,y)
color <- brewer.pal(10, "Paired")
color <- c("#FFFFFF", color)

execute <- function(){
    for(step in 1:last){
        g <- ggplot(
            data = empty,
            aes(x = x, y = y)
        )
        g <- g + coord_fixed()
        g <- g + theme(axis.ticks = element_blank(), axis.text = element_blank())
        g <- g + theme(axis.title = element_blank())
        g <- g + scale_y_reverse()
        g <- g + ggtitle(step - 1)
        for(y in 0:4){
            for(x in 0:3){
                piece <- b[step, 2 + x + 4 * y]
                g <- g + annotate("rect", xmin = x, xmax = x+1, ymin = y, ymax = y+1, fill = color[piece+1])
            }
        }
        plot(g)
        setTxtProgressBar(pb, step)
    }
}

saveVideo({
    ani.options(interval = 0.5)
    execute()
},video.name = "anime.mp4", other.opts = "-pix_fmt yuv420p -b 1000k"
)