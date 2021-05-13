#include <mainwindow.h>



int MainWindow::robot(int color)
{
    int weight[20][20] = {{0}};     //权重数组
    int max = 0,max_index = 1010;      //权重最大值
//    int test[18] = {0};           //用于测试ai权值判断准确性，与robot函数最后的qDebug() << test[]  连用

    for(int i = 0;i < 20;i++)       //先初步筛选，等会儿需要判断权重的格子：这些格子的周围有已经下过的格子
    {
        for(int j = 0;j < 20;j++)
        {
            if(situations[i][j] != 0)
            {
                if(i - 1 > 0 && j - 1 > 0)      weight[i-1][j-1] = 1;
                if(i + 1 < 20 && j + 1 < 20)    weight[i+1][j+1] = 1;
                if(i - 1 > 0 && j + 1 < 20)    weight[i-1][j+1] = 1;
                if(i + 1 < 20 && j - 1 > 0)    weight[i+1][j-1] = 1;
                if(i + 1 < 20)      weight[i+1][j] = 1;
                if(i - 1 > 0)       weight[i-1][j] = 1;
                if(j - 1 > 0)       weight[i][j-1] = 1;
                if(j + 1 < 20)      weight[i][j+1] = 1;
            }
        }
    }
    for(int i = 0;i < 20;i++)           //已经下过的格子去掉
    {
        for(int j = 0;j < 20;j++)
        {
            if(situations[i][j] != 0)
            {
                weight[i][j] = 0;
            }
        }
    }

    int enemy;                      //判断敌方是黑棋还是白棋
    if(color == 1)                  //若己方为黑，则敌方为白
    {
        enemy = 2;
    }
    else                            //若己方为白，则敌方为黑
    {
        enemy = 1;
    }

    /*
     * 下面是核心算法，涉及权重算法以及博弈论的相关原理
     * 步骤一、假设该点已经下了友方棋子，即令 situations[i][j] = color（color中存放了当前
     *        友方棋子颜色："1"为黑色，"2"为白色）
     * 步骤二、在假设的前提下，先判断四个方向有没有成六（因为六不用考虑活、冲，较好判断，且六是
     *        决定性的）
     * 步骤三、先判断横着方向的活
     * 步骤四、再判断横着方向的冲
     * 步骤五、接着在横着方向，所有存在的活、冲棋型中，挑选权重最大的一个作为该方向的代表赋入
     *        friend_true数组中，权重大小为：活5 > 活4 > 冲5 > 活3 > 冲4 > 活2 > 冲3 >
     *        冲2
     *
     *          注意：步骤五中 “活4 > 冲5” 是因为这是假设已经下了这一点后构成的棋型，而不是
     *               假设该点是空格时周围构成的棋型
     *
     * 步骤六、对竖直、左斜、右斜方向重复步骤三、四、五
     *
     *      中途小结：此时友方在此点的判断全部存入friend_true数组。下面运用博弈，判断
     *              对于敌方而言，此点的重要性；该结果将会存入enemy_true数组中
     *
     * 步骤七、假设该点已经下了敌方棋子，即令 situations[i][j] = enemy（enemy中存放了当前
     *        敌方棋子颜色："1"为黑色，"2"为白色）
     * 步骤八、将enemy中的值与color中的值暂时调换位置，这样就可以用 判断该点对友方重要性的代
     *        码（即步骤二到步骤六的代码） 来判断该点对于敌方的重要性
     * 步骤九、假设该点下了敌方的棋子，重复步骤二至六，以判断该点对于敌方的重要性
     * 步骤十、enemy与color的值互换--交换回真实值
     * 步骤十一、至此，所有敌方友方判断均已结束，并分别放置在friend_true与enemy_true数组中，
     *         我们对这20种棋型乘以它们在该点处存在的个数，再乘以不同权值，并相加。最终我们会
     *         得到该点的总权值
     *
     *              以上，核心算法结束
     *
     * 注：后又添加了available的算法，在下面有详细说明
    */

    for(int i = 0;i < 20;i++)       //开始判断权重
    {
        for(int j = 0;j < 20;j++)
        {
            if(weight[i][j] == 1)   //如果是之前筛选中的对象，就开始分析该点权重
            {
                situations[i][j] = color;       //先假定该点有了己方棋子，判断若下了该点，我方好处

                int a = i,b = j;

                /*活棋类型*/
                int true_two = 0,true_three = 0,true_four = 0,true_five = 0,true_six = 0;
                /*冲棋类型*/
                int fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

                int space = 0;
                int temp_count = 0;
                int friend_true[9] = {0};       //这里一开始取名字失误了！应该表示所有友方类型。其中存储：如果友方下了这个空点，友方构成多少个活、多少个冲
                int enemy_true[9] = {0};        //这里表示敌方。其中存储：如果敌方下了这个空点，敌方构成多少个活、多少个冲
                /*
                 * [0]表示冲2，[1]表示冲3，[2]表示冲4，[3]表示冲5，[4]表示活2，
                 * [5]表示活3，[6]表示活4，[7]表示活5，[8]表示活6或者冲6（活6冲6没区别）
                */

                int available = 0;    //available变量用于判断该点处向左右是否足够有6个空间
                                      //即表明，该点向左的第一个敌人，到向右的第一个敌人之间
                                      //格数；若格数小于6，那么该点处下棋对于我方无意义（因为
                                      //成不了6）；对于敌方，若我方两个棋子之间间隔小于6，那
                                      //么，敌方下这里也没有意义



                for(a = i;situations[a][j] == color && a >= 1;a--)
                {
                    temp_count++;
                }
                for(a = i+1;situations[a][j] == color && a <= 19;a++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j;situations[i][b] == color && b >= 1;b--)
                {
                    temp_count++;
                }
                for(b = j+1;situations[i][b] == color && b <= 19;b++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j,a = i;situations[a][b] == color && a >= 1 && b >= 1;b--,a--)
                {
                    temp_count++;
                }
                for(b = j + 1,a = i + 1;situations[a][b] == color && a <= 19 && b <= 19;b++,a++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j,a = i;situations[a][b] == color && b >= 1 && a <= 19;b--,a++)
                {
                    temp_count++;
                }
                for(b = j + 1,a = i - 1;situations[a][b] == color && b <= 19 && a >= 1;b++,a--)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                if(true_six > 0)
                {
                    friend_true[8] += true_six;
                    true_six = 0;
                }




                available = 0;
                for (a = i;a < i + 6 && situations[a][j] != enemy && a < 20;a++)
                {
                    available++;
                }
                for (a = i - 1;a > i - 6 && situations[a][j] != enemy && a > 0;a--)
                {
                    available++;
                }
                if(available < 6) goto skip_horizon;

                for (a = i - 5;a <= i;a++)
                {
                    if(a <= 0) a = 1;

                    int m = a;
                    while(space <= 1 && m < 20 && temp_count < 5)
                    {
                        if(situations[a-1][j] != 0 || a == 1) break;
                        if(situations[a][j] != color) break;

                        if(situations[m][j] == 0) space++;
                        else if(situations[m][j] == color && m < 19) temp_count++;
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][j] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3;a <= i;a++)
                {
                    if(a <= 0) a = 1;

                    int m = a;
                    while(space <= 1 && m < 20)
                    {
                        if(situations[a-1][j] != 0 || a == 1) break;
                        if(situations[a][j] != color) break;

                        if(situations[m][j] == 0) space++;
                        else if(situations[m][j] == color && m < 19) temp_count++;
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (a = i - 5;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 4;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 3;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 2;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    friend_true[7]++;
                }
                else if(true_four > 0)
                {
                    friend_true[6]++;
                }
                else if(fake_five > 0)
                {
                    friend_true[3]++;
                }
                else if(true_three > 0)
                {
                    friend_true[5]++;
                }
                else if(fake_four > 0)
                {
                    friend_true[2]++;
                }
                else if(true_two > 0)
                {
                    friend_true[4]++;
                }
                else if(fake_three > 0)
                {
                    friend_true[1]++;
                }
                else if(fake_two > 0)
                {
                    friend_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;
skip_horizon:



                available = 0;
                for (b = j;b < j + 6 && situations[i][b] != enemy && b < 20;b++)
                {
                    available++;
                }
                for (b = j - 1;b > j - 6 && situations[i][b] != enemy && b > 0;b--)
                {
                    available++;
                }
                if(available < 6) goto skip_upright;

                for (b = j - 5;b <= j;b++)
                {
                    if(b <= 0) b = 1;

                    int m = b;
                    while(space <= 1 && m < 20 && temp_count < 5)
                    {
                        if(situations[i][b-1] != 0 || b == 1) break;
                        if(situations[i][b] != color) break;

                        if(situations[i][m] == 0) space++;
                        else if(situations[i][m] == color && m < 19) temp_count++;
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[i][m] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (b = j - 3;b <= j;b++)
                {
                    if(b <= 0) b = 1;

                    int m = b;
                    while(space <= 1 && m < 20)
                    {
                        if(situations[i][b-1] != 0 || b == 1) break;
                        if(situations[i][b] != color) break;

                        if(situations[i][m] == 0) space++;
                        else if(situations[i][m] == color && m < 19) temp_count++;
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j - 5;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 4;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 3;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 2;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    friend_true[7]++;
                }
                else if(true_four > 0)
                {
                    friend_true[6]++;
                }
                else if(fake_five > 0)
                {
                    friend_true[3]++;
                }
                else if(true_three > 0)
                {
                    friend_true[5]++;
                }
                else if(fake_four > 0)
                {
                    friend_true[2]++;
                }
                else if(true_two > 0)
                {
                    friend_true[4]++;
                }
                else if(fake_three > 0)
                {
                    friend_true[1]++;
                }
                else if(fake_two > 0)
                {
                    friend_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;
skip_upright:



                available = 0;
                for (b = j,a = i;b < j + 6 && situations[a][b] != enemy && a < 20 && b < 20;a++,b++)
                {
                    available++;
                }
                for (b = j - 1,a = i - 1;b > j - 6 && situations[a][b] != enemy && a > 0 && b > 0;a--,b--)
                {
                    available++;
                }
                if(available < 6) goto skip_left_oblique;

                for (a = i - 5,b = j - 5;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n < 20 && temp_count < 5)
                    {
                        if(situations[a-1][b-1] != 0 || a == 1 || b == 1) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n < 19) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                        {
                            break;
                        }
                        m++;
                        n++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][n] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3,b = j - 3;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n < 20)
                    {
                        if(situations[a-1][b-1] != 0 || a == 1 || b == 1) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n < 19) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                        {
                            break;
                        }
                        m++;
                        n++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j - 5,a = i - 5;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 4,a = i - 4;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 3,a = i - 3;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 2,a = i - 2;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    friend_true[7]++;
                }
                else if(true_four > 0)
                {
                    friend_true[6]++;
                }
                else if(fake_five > 0)
                {
                    friend_true[3]++;
                }
                else if(true_three > 0)
                {
                    friend_true[5]++;
                }
                else if(fake_four > 0)
                {
                    friend_true[2]++;
                }
                else if(true_two > 0)
                {
                    friend_true[4]++;
                }
                else if(fake_three > 0)
                {
                    friend_true[1]++;
                }
                else if(fake_two > 0)
                {
                    friend_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

skip_left_oblique:



                available = 0;
                for (b = j,a = i;b < j + 6 && situations[a][b] != enemy && b < 20 && a > 0;a--,b++)
                {
                    available++;
                }
                for (b = j - 1,a = i + 1;b > j - 6 && situations[a][b] != enemy && b > 0 && a < 20;a++,b--)
                {
                    available++;
                }
                if(available < 6) goto skip_right_oblique;

                for (a = i - 5,b = j + 5;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n > 0 && temp_count < 5)
                    {
                        if(situations[a-1][b+1] != 0 || a == 1 || b == 19) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n > 1) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                        {
                            break;
                        }
                        m++;
                        n--;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][n] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3,b = j + 3;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n > 0)
                    {
                        if(situations[a-1][b+1] != 0 || a == 1 || b == 19) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n > 1) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                        {
                            break;
                        }
                        m++;
                        n--;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j + 5,a = i - 5;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 4,a = i - 4;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 3,a = i - 3;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 2,a = i - 2;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }


                if(true_five > 0)
                {
                    friend_true[7]++;
                }
                else if(true_four > 0)
                {
                    friend_true[6]++;
                }
                else if(fake_five > 0)
                {
                    friend_true[3]++;
                }
                else if(true_three > 0)
                {
                    friend_true[5]++;
                }
                else if(fake_four > 0)
                {
                    friend_true[2]++;
                }
                else if(true_two > 0)
                {
                    friend_true[4]++;
                }
                else if(fake_three > 0)
                {
                    friend_true[1]++;
                }
                else if(fake_two > 0)
                {
                    friend_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

skip_right_oblique:



                situations[i][j] = enemy;
                int swap = enemy;
                enemy = color;
                color = swap;

                for(a = i;situations[a][j] == color && a >= 1;a--)
                {
                    temp_count++;
                }
                for(a = i+1;situations[a][j] == color && a <= 19;a++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j;situations[i][b] == color && b >= 1;b--)
                {
                    temp_count++;
                }
                for(b = j+1;situations[i][b] == color && b <= 19;b++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j,a = i;situations[a][b] == color && a >= 1 && b >= 1;b--,a--)
                {
                    temp_count++;
                }
                for(b = j + 1,a = i + 1;situations[a][b] == color && a <= 19 && b <= 19;b++,a++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j,a = i;situations[a][b] == color && b >= 1 && a <= 19;b--,a++)
                {
                    temp_count++;
                }
                for(b = j + 1,a = i - 1;situations[a][b] == color && b <= 19 && a >= 1;b++,a--)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                if(true_six > 0)
                {
                    enemy_true[8] += true_six;
                    true_six = 0;
                }


                available = 0;
                for (a = i;a < i + 6 && situations[a][j] != enemy && a < 20;a++)
                {
                    available++;
                }
                for (a = i - 1;a > i - 6 && situations[a][j] != enemy && a > 0;a--)
                {
                    available++;
                }
                if(available < 6) goto enemy_skip_horizon;

                for (a = i - 5;a <= i;a++)
                {
                    if(a <= 0) a = 1;

                    int m = a;
                    while(space <= 1 && m < 20 && temp_count < 5)
                    {
                        if(situations[a-1][j] != 0 || a == 1) break;
                        if(situations[a][j] != color) break;

                        if(situations[m][j] == 0) space++;
                        else if(situations[m][j] == color && m < 19) temp_count++;
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][j] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3;a <= i;a++)
                {
                    if(a <= 0) a = 1;

                    int m = a;
                    while(space <= 1 && m < 20)
                    {
                        if(situations[a-1][j] != 0 || a == 1) break;
                        if(situations[a][j] != color) break;

                        if(situations[m][j] == 0) space++;
                        else if(situations[m][j] == color && m < 19) temp_count++;
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (a = i - 5;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 4;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 3;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 2;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    enemy_true[7]++;
                }
                else if(true_four > 0)
                {
                    enemy_true[6]++;
                }
                else if(fake_five > 0)
                {
                    enemy_true[3]++;
                }
                else if(true_three > 0)
                {
                    enemy_true[5]++;
                }
                else if(fake_four > 0)
                {
                    enemy_true[2]++;
                }
                else if(true_two > 0)
                {
                    enemy_true[4]++;
                }
                else if(fake_three > 0)
                {
                    enemy_true[1]++;
                }
                else if(fake_two > 0)
                {
                    enemy_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

enemy_skip_horizon:



                available = 0;
                for (b = j;b < j + 6 && situations[i][b] != enemy && b < 20;b++)
                {
                    available++;
                }
                for (b = j - 1;b > j - 6 && situations[i][b] != enemy && b > 0;b--)
                {
                    available++;
                }
                if(available < 6) goto enemy_skip_upright;

                for (b = j - 5;b <= j;b++)
                {
                    if(b <= 0) b = 1;

                    int m = b;
                    while(space <= 1 && m < 20 && temp_count < 5)
                    {
                        if(situations[i][b-1] != 0 || b == 1) break;
                        if(situations[i][b] != color) break;

                        if(situations[i][m] == 0) space++;
                        else if(situations[i][m] == color && m < 19) temp_count++;
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[i][m] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (b = j - 3;b <= j;b++)
                {
                    if(b <= 0) b = 1;

                    int m = b;
                    while(space <= 1 && m < 20)
                    {
                        if(situations[i][b-1] != 0 || b == 1) break;
                        if(situations[i][b] != color) break;

                        if(situations[i][m] == 0) space++;
                        else if(situations[i][m] == color && m < 19) temp_count++;
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j - 5;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 4;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 3;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 2;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    enemy_true[7]++;
                }
                else if(true_four > 0)
                {
                    enemy_true[6]++;
                }
                else if(fake_five > 0)
                {
                    enemy_true[3]++;
                }
                else if(true_three > 0)
                {
                    enemy_true[5]++;
                }
                else if(fake_four > 0)
                {
                    enemy_true[2]++;
                }
                else if(true_two > 0)
                {
                    enemy_true[4]++;
                }
                else if(fake_three > 0)
                {
                    enemy_true[1]++;
                }
                else if(fake_two > 0)
                {
                    enemy_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

enemy_skip_upright:



                available = 0;
                for (b = j,a = i;b < j + 6 && situations[a][b] != enemy && a < 20 && b < 20;a++,b++)
                {
                    available++;
                }
                for (b = j - 1,a = i - 1;b > j - 6 && situations[a][b] != enemy && a > 0 && b > 0;a--,b--)
                {
                    available++;
                }
                if(available < 6) goto enemy_skip_left_oblique;

                for (a = i - 5,b = j - 5;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n < 20 && temp_count < 5)
                    {
                        if(situations[a-1][b-1] != 0 || a == 1 || b == 1) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n < 19) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                        {
                            break;
                        }
                        m++;
                        n++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][n] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3,b = j - 3;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n < 20)
                    {
                        if(situations[a-1][b-1] != 0 || a == 1 || b == 1) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n < 19) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                        {
                            break;
                        }
                        m++;
                        n++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j - 5,a = i - 5;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 4,a = i - 4;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 3,a = i - 3;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 2,a = i - 2;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    enemy_true[7]++;
                }
                else if(true_four > 0)
                {
                    enemy_true[6]++;
                }
                else if(fake_five > 0)
                {
                    enemy_true[3]++;
                }
                else if(true_three > 0)
                {
                    enemy_true[5]++;
                }
                else if(fake_four > 0)
                {
                    enemy_true[2]++;
                }
                else if(true_two > 0)
                {
                    enemy_true[4]++;
                }
                else if(fake_three > 0)
                {
                    enemy_true[1]++;
                }
                else if(fake_two > 0)
                {
                    enemy_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

enemy_skip_left_oblique:


                available = 0;
                for (b = j,a = i;b < j + 6 && situations[a][b] != enemy && b < 20 && a > 0;a--,b++)
                {
                    available++;
                }
                for (b = j - 1,a = i + 1;b > j - 6 && situations[a][b] != enemy && b > 0 && a < 20;a++,b--)
                {
                    available++;
                }
                if(available < 6) goto enemy_skip_right_oblique;

                for (a = i - 5,b = j + 5;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n > 0 && temp_count < 5)
                    {
                        if(situations[a-1][b+1] != 0 || a == 1 || b == 19) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n > 1) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                        {
                            break;
                        }
                        m++;
                        n--;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][n] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3,b = j + 3;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n > 0)
                    {
                        if(situations[a-1][b+1] != 0 || a == 1 || b == 19) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n > 1) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                        {
                            break;
                        }
                        m++;
                        n--;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j + 5,a = i - 5;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 4,a = i - 4;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 3,a = i - 3;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 2,a = i - 2;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }


                if(true_five > 0)
                {
                    enemy_true[7]++;
                }
                else if(true_four > 0)
                {
                    enemy_true[6]++;
                }
                else if(fake_five > 0)
                {
                    enemy_true[3]++;
                }
                else if(true_three > 0)
                {
                    enemy_true[5]++;
                }
                else if(fake_four > 0)
                {
                    enemy_true[2]++;
                }
                else if(true_two > 0)
                {
                    enemy_true[4]++;
                }
                else if(fake_three > 0)
                {
                    enemy_true[1]++;
                }
                else if(fake_two > 0)
                {
                    enemy_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

enemy_skip_right_oblique:


                swap = enemy;
                enemy = color;
                color = swap;


                situations[i][j] = 0;

                weight[i][j] = friend_true[8] * 15000 + enemy_true[8] * 7450    //六
                             + friend_true[7] * 3700 + enemy_true[7] * 3000     //活五
                             + friend_true[6] * 1200 + enemy_true[6] * 1100      //活四
                             + friend_true[3] * 700 + enemy_true[3] * 600       //冲五
                             + friend_true[5] * 400 + enemy_true[5] * 300       //活三
                             + friend_true[2] * 300 + enemy_true[2] * 200       //冲四
                             + friend_true[4] * 200 + enemy_true[4] * 100       //活二
                             + friend_true[1] * 100 + enemy_true[1] * 50        //冲三
                             + friend_true[0] * 50 + enemy_true[0] * 20;        //冲二

                if( max < weight[i][j] )
                {
                    if(times % 2 == 1 && judge_rule(i,j) != 0)
                    {
                        continue;
                    }
                    max = weight[i][j];
                    max_index = i*100 + j;
//                    test[0] = friend_true[0];
//                    test[1] = friend_true[1];
//                    test[2] = friend_true[2];
//                    test[3] = friend_true[3];
//                    test[4] = friend_true[4];
//                    test[5] = friend_true[5];
//                    test[6] = friend_true[6];
//                    test[7] = friend_true[7];
//                    test[8] = friend_true[8];
//                    test[9] = enemy_true[0];
//                    test[10] = enemy_true[1];
//                    test[11] = enemy_true[2];
//                    test[12] = enemy_true[3];
//                    test[13] = enemy_true[4];
//                    test[14] = enemy_true[5];
//                    test[15] = enemy_true[6];
//                    test[16] = enemy_true[7];
//                    test[17] = enemy_true[8];
                }
            }
        }
    }

    if(max == 0 && times != 1 && times != 0)      //不是第一步，且所有格子权值全部为0时，选取周围空间最大的空格（每个格子周围最多8个空格子）
    {
        int free_space = 1;         //赋值为1，防止此时还出现最大空余空间还是0的情况（即所有剩下空格的四周都没有任何空格）
        int max_free_space = 0;     //初始化最大剩余空间为0
        for (int i = 1;i <= 19;i++)
        {
            for (int j = 1;j <= 19;j++)
            {
                if(situations[i][j] == 0)
                {
                    if(i - 1 > 0 && j - 1 > 0 && situations[i-1][j-1] == 0)      free_space++;
                    if(i + 1 < 20 && j + 1 < 20 && situations[i+1][j+1] == 0)    free_space++;
                    if(i - 1 > 0 && j + 1 < 20 && situations[i-1][j+1] == 0)    free_space++;
                    if(i + 1 < 20 && j - 1 > 0 && situations[i+1][j-1] == 0)    free_space++;
                    if(i + 1 < 20 && situations[i+1][j] == 0)      free_space++;
                    if(i - 1 > 0 && situations[i-1][j] == 0)       free_space++;
                    if(j - 1 > 0 && situations[i][j-1] == 0)       free_space++;
                    if(j + 1 < 20 && situations[i][j+1] == 0)      free_space++;

                    if(max_free_space < free_space)
                    {
                        max_free_space = free_space;
                        max_index = i*100 + j;
                    }

                    free_space = 0;
                }
            }
        }
    }

    situations[max_index/100][max_index%100] = color;


//    qDebug() << "己方：活：" << '\n' << "六 " <<test[8] << " 五 " << test[7] << " 四 " << test[6] << " 三 "<< test[5] << " 二 " << test[4] << '\n';
//    qDebug() << "己方：冲：" << '\n' << "五 " <<test[3] << " 四 " << test[2] << " 三 " << test[1] << " 二 " << test[0] << '\n';

//    qDebug() << "敌方：活：" << '\n' << "六 " <<test[17] << " 五 " << test[16] << " 四 " << test[15] << " 三 "<< test[14] << " 二 " << test[13] << '\n';
//    qDebug() << "敌方：冲：" << '\n' << "五 " <<test[12] << " 四 " << test[11] << " 三 " << test[10] << " 二 " << test[9] << '\n';
//    qDebug() << max;
    return max_index;
}
