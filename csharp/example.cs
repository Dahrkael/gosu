using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GosuNET.Gosu;
using GosuTutorial;

namespace GosuTutorial
{
    enum ZOrder
    {
        zBackground,
        zStars,
        zPlayer,
        zUI
    };

    public class Star
    {
        Image image;
        List<Image> animation;
        Color color;
        double posX, posY;


        public Star(List<Image> animation)
        {
            this.animation = animation;
            image = new Image("media/star1.png");
            double red = GosuNET.Gosu.Math.Random(40, 255);
            double green = GosuNET.Gosu.Math.Random(40, 255);
            double blue = GosuNET.Gosu.Math.Random(40, 255);
            color = new Color(255, (byte)red, (byte)green, (byte)blue);

            posX = GosuNET.Gosu.Math.Random(0, 640);
            posY = GosuNET.Gosu.Math.Random(0, 480);
        }

        public double x() { return posX; }
        public double y() { return posY; }

        public void draw()
        {
            //Image image = animation[(int)(Timing.Milliseconds() / 100 % animation.Count)];
            image.Draw(posX - image.Width / 2.0, posY - image.Height / 2.0, (double)ZOrder.zStars, 1, 1, color, (AlphaMode)1);
        }
    }

    public class Player
    {
        Image image;
        Sample beep;
        double posX, posY, velX, velY, angle;
        UInt32 score;

        public Player()
        {
            image = new Image("media/Starfighter.bmp");
            beep = new Sample("media/Beep.wav");
            posX = posY = velX = velY = angle = 0;
            score = 0;
        }

        public UInt32 getScore()
        {
            return score;
        }

        public void warp(double x, double y)
        {
            posX = x;
            posY = y;
        }

        public void turnLeft()
        {
            angle -= 4.5;
        }

        public void turnRight()
        {
            angle += 4.5;
        }

        public void accelerate()
        {
            velX += GosuNET.Gosu.Math.OffsetX(angle, 0.5);
            velY += GosuNET.Gosu.Math.OffsetY(angle, 0.5);
        }

        public void move()
        {
            posX = GosuNET.Gosu.Math.Wrap(posX + velX, 0.0, 640.0);
            posY = GosuNET.Gosu.Math.Wrap(posY + velY, 0.0, 480.0);

            velX *= 0.95;
            velY *= 0.95;
        }

        public void draw()
        {
            image.DrawRot(posX, posY, (double)ZOrder.zPlayer, angle, 0.5, 0.5, 1, 1, Color.WHITE, 0);
        }

        public void collectStars(ref List<Star> stars)
        {
            int amount = stars.RemoveAll(s => GosuNET.Gosu.Math.Distance(posX, posY, s.x(), s.y()) < 35);
            if (amount > 0)
            {
                score += (uint)(amount * 10);
                beep.Play();
            }
        }
    }

class GameWindow : Window
{
    Image backgroundImage;
    List<Image> starAnim;
    Font font;

    Player player;
    List<Star> stars;


    public GameWindow() : base(640, 480)
    {
        font = new Font(20);
        SetCaption("Gosu Tutorial Game");

        backgroundImage = new Image("media/Space.png", (uint)ImageFlags.IfTileable);
        //starAnim = loadTiles("media/Star.png", 25, 25);
        stars = new List<Star>();
        player = new Player();
        player.warp(320, 240);
    }

    public override void Update()
    {
        if (Input.Down(ButtonName.KbLeft) || Input.Down(ButtonName.GpLeft))
            player.turnLeft();
        if (Input.Down(ButtonName.KbRight) || Input.Down(ButtonName.GpRight))
            player.turnRight();
        if (Input.Down(ButtonName.KbUp) || Input.Down(ButtonName.GpButton0))
            player.accelerate();

        player.move();
        player.collectStars(ref stars);

        if (GosuNET.Gosu.Math.Random(0, 30) > 20 && stars.Count < 25)
        stars.Add(new Star(starAnim));
    }

    public override void Draw()
    {
        player.draw();
        backgroundImage.Draw(0, 0, (double)ZOrder.zBackground, 1, 1, Color.WHITE, 0);
        stars.ForEach(s => s.draw());
        font.Draw("Score: " + player.getScore(), 10, 10, (double)ZOrder.zUI, 1, 1, Color.YELLOW, 0);
    }

    public override void ButtonDown(Button btn)
    {
        //if (btn == ButtonName.KbEscape) { }
    }
}


class Program
{
    static void Main(string[] args)
    {
        GameWindow window = new GameWindow();
        window.Show();
    }
}
}
