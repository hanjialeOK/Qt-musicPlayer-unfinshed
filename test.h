#ifndef TEST_H
#define TEST_H

#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QBoxLayout>
#include <QQueue>
#include <qlist.h>
#include <QScrollArea>
#include <QtMath>
#include <QPainter>

class MyLabel;
class Tree;

class test : public QWidget
{
    Q_OBJECT
public:
    explicit test(QWidget *parent = nullptr);

private:
    void initUi();

protected:
};

class Tree
{
public:
    explicit Tree();

    struct _SyntaxTreeNode {
        struct _SyntaxTreeNode * parent;
        struct _SyntaxTreeNode * children[1000];
        int children_num;
        char name[30];
        QPoint pos;
        int offset;
    };
    typedef struct _SyntaxTreeNode SyntaxTreeNode;

    struct _SyntaxTree {
        SyntaxTreeNode * root;
    };
    typedef struct _SyntaxTree SyntaxTree;

    //void printSyntaxTreeNode(FILE * fout, SyntaxTreeNode * node, int level);
    static SyntaxTreeNode * newSyntaxTreeNodeNoName();
    static SyntaxTreeNode * newSyntaxTreeNode(const char * name);
    static SyntaxTreeNode * newSyntaxTreeNodeFromNum(const int num);
    static int SyntaxTreeNode_AddChild(SyntaxTreeNode * parent, SyntaxTreeNode * child);
    static void deleteSyntaxTreeNodeNoRecur(SyntaxTreeNode * node);
    static void deleteSyntaxTreeNode(SyntaxTreeNode * node, int recursive);
    static SyntaxTree * newSyntaxTree();
    static void deleteSyntaxTree(SyntaxTree * tree);
    static void printSyntaxTree(FILE * fout, SyntaxTree * tree);

private:
    static void printSyntaxTreeNode(FILE * fout, SyntaxTreeNode * node, int level);
};

class MyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MyLabel(QWidget *parent = nullptr, Tree::SyntaxTree *t = nullptr);

private:
    int rowSize;
    int columnSize;
    int nodeHeight;
    int treeHeight;
    int maxChildNum;
    Tree::SyntaxTree *tree;
    QPoint getMidUpPos(QPoint pos) const {return QPoint(pos.x() * columnSize - columnSize / 2, (pos.y() - 1) * rowSize);}
    QPoint getMidBottomPos(QPoint pos) const {return QPoint(pos.x() * columnSize - columnSize / 2, pos.y() * rowSize);}
    QPoint getLeftUpPos(QPoint pos) const {return QPoint((pos.x() - 1) * columnSize, (pos.y() - 1) * rowSize);}
    int getTreeHeight(Tree::SyntaxTreeNode *node);
    int getMaxChildNum(Tree::SyntaxTreeNode *node);
    void initOffset(Tree::SyntaxTreeNode *node);

protected:
    void paintEvent(QPaintEvent *) override;
};

#endif // TEST_H
