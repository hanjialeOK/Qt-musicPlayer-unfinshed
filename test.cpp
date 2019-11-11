#include "test.h"

Tree::Tree(){}

Tree::SyntaxTreeNode * Tree::newSyntaxTreeNodeNoName()
{
    return newSyntaxTreeNode(nullptr);
}

Tree::SyntaxTreeNode * Tree::newSyntaxTreeNode(const char * name)
{
    SyntaxTreeNode * newNode = (SyntaxTreeNode *)malloc(sizeof(SyntaxTreeNode));
    if (name)
        strcpy(newNode->name, name);
    else
        newNode->name[0] = '\0';
    newNode->children_num = 0;

    //printf("%s\n", name);

    return newNode;
}

Tree::SyntaxTreeNode * Tree::newSyntaxTreeNodeFromNum(const int num)
{
    SyntaxTreeNode * newNode = newSyntaxTreeNodeNoName();
    sprintf(newNode->name, "%d", num);
    newNode->children_num = 0;
    return newNode;
}

int Tree::SyntaxTreeNode_AddChild(SyntaxTreeNode * parent, SyntaxTreeNode * child)
{
    if (!parent || !child)	return -1;
    parent->children[parent->children_num++] = child;
    return parent->children_num;
}

void Tree::deleteSyntaxTreeNodeNoRecur(SyntaxTreeNode * node)
{
    deleteSyntaxTreeNode(node, 0);
}

void Tree::deleteSyntaxTreeNode(SyntaxTreeNode * node, int recursive)
{
    if (!node)	return;

    int i;
    if (recursive) {
        for (i = 0; i < node->children_num; i++) {
            deleteSyntaxTreeNode(node->children[i], 1);
        }
    }
    free(node);
}

Tree::SyntaxTree * Tree::newSyntaxTree()
{
    SyntaxTree * newTree = (SyntaxTree *)malloc(sizeof(SyntaxTree));
    newTree->root = nullptr;
    return newTree;
}

void Tree::deleteSyntaxTree(SyntaxTree * tree)
{
    if (!tree)	return;

    if (tree->root) {
        deleteSyntaxTreeNode(tree->root, 1);
    }
    free(tree);
}

void Tree::printSyntaxTreeNode(FILE * fout, SyntaxTreeNode * node, int level)
{
    // check if "node" empty pointer
    if (!node)	return;
    // print myself
    int i;
    for (i = 0; i < level; i++) {
        fprintf(fout, "|  ");
    }
    fprintf(fout, ">--%s %s\n", (node->children_num ? "+" : "*"), node->name);

    for (i = 0; i < node->children_num; i++) {
        printSyntaxTreeNode(fout, node->children[i], level + 1);
    }
}

void Tree::printSyntaxTree(FILE * fout, SyntaxTree * tree)
{
    if (!fout)	return;
    printSyntaxTreeNode(fout, tree->root, 0);
}

//****************************MyLabel**************************MyLabel*************************MyLabel**********************************//

MyLabel::MyLabel(QWidget *parent, Tree::SyntaxTree *t)
    : QLabel(parent)
    , rowSize(20)
    , columnSize(20)
    , nodeHeight(3)
    , tree(t)
{
    initOffset(tree->root);
    treeHeight = getTreeHeight(tree->root);
    maxChildNum = getMaxChildNum(tree->root);
}

int MyLabel::getTreeHeight(Tree::SyntaxTreeNode *node)
{
    if(!node) return 0;
    if(node->children_num == 0) return 1;
    int h = 0;
    for(int i = 0; i < node->children_num; i++){
        h = qMax(h, getTreeHeight(node->children[i]));
    }
    return h + 1;
}

int MyLabel::getMaxChildNum(Tree::SyntaxTreeNode *node)
{
    if(!node || node->children_num == 0) return 0;
    int n = node->children_num;
    QQueue<Tree::SyntaxTreeNode *> queue;
    queue.enqueue(node);
    qDebug()<<node->offset;
    while(!queue.empty()){
        Tree::SyntaxTreeNode *p = queue.dequeue();
        for(int i = 0; i < p->children_num; i++){
            n = qMax(n, p->children[i]->children_num);
            queue.enqueue(p->children[i]);
            qDebug()<<p->children[i]->offset;
        }
    }
    return n;
}

void MyLabel::initOffset(Tree::SyntaxTreeNode *node)
{
    if(node->children_num == 0){
        node->offset = 0;
    }
    else{
        int MaxOffsetInChild = 0;
        for(int i = 0; i < node->children_num; i++){
            initOffset(node->children[i]);
            MaxOffsetInChild = qMax(MaxOffsetInChild, node->children[i]->offset);
        }
        if(node->children_num == 1) node->offset = MaxOffsetInChild + 1;
        else if(MaxOffsetInChild == 0) node->offset = node->children_num - 1;
        else node->offset = MaxOffsetInChild * (node->children_num - 1) + node->children_num / 2 * 2 - 1;
    }
}

void MyLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.setPen(Qt::NoPen);
    painter.setPen(QPen(QColor(200, 200, 200)));
    painter.setFont(QFont("Microsoft YaHei", 6, QFont::Normal));

    for(int i = 0; i < width(); i += columnSize)
        painter.drawLine(i, 0, i, height());
    for(int i = 0; i < height(); i += rowSize)
        painter.drawLine(0, i, width(), i);

//    int n = 0;
//    QQueue<QPoint> queue;

    int offset_array[7] = {0, 0, 0, 0, 0, 0, 0};

    QQueue<Tree::SyntaxTreeNode *> queue;
    Tree::SyntaxTreeNode *p;
    p = tree->root;
    p->pos = QPoint(width() / columnSize / 2, 1);

    painter.setPen(QPen(QColor(180, 0, 0)));
    QRect nodeRect = QRect(getLeftUpPos(p->pos).x(), getLeftUpPos(p->pos).y(), columnSize, rowSize);
    painter.drawEllipse(nodeRect);
    painter.drawText(nodeRect, p->name, QTextOption(Qt::AlignCenter));

    queue.enqueue(p);

    while(!queue.empty()){
        p = queue.dequeue();
        QPoint pos = p->pos;
        int offset = p->offset;

        //int n = (pos.y() - 1) / nodeHeight;
        //int offset = 16 / static_cast<int>(qPow(maxChildNum, n));
        //if(offset == 0) break;

        switch (p->children_num) {
        case 1:
            offset_array[0] = 0;
            break;
        case 2:
            offset_array[0] = -offset;
            offset_array[1] = offset;
            break;
        case 3:
            offset_array[0] = -offset;
            offset_array[1] = 0;
            offset_array[2] = offset;
            break;
        case 4:
            offset_array[0] = -offset;
            offset_array[1] = -static_cast<int>(offset / 3.0f + 0.5f);
            offset_array[2] = static_cast<int>(offset / 3.0f + 0.5f);
            offset_array[3] = offset;
            break;
        case 5:
            offset_array[0] = -offset;
            offset_array[1] = -static_cast<int>(offset * 2 / 4.0f + 0.5f);
            offset_array[2] = 0;
            offset_array[3] = static_cast<int>(offset * 2 / 4.0f + 0.5f);
            offset_array[4] = offset;
            break;
        case 6:
            offset_array[0] = -offset;
            offset_array[1] = -static_cast<int>(offset * 3 / 5.0f + 0.5f);
            offset_array[2] = -static_cast<int>(offset / 5.0f + 0.5f);
            offset_array[3] = static_cast<int>(offset / 5.0f + 0.5f);
            offset_array[4] = static_cast<int>(offset * 3 / 5.0f + 0.5f);
            offset_array[5] = offset;
            break;
        case 7:
            offset_array[0] = -offset;
            offset_array[1] = -static_cast<int>(offset * 4 / 6.0f + 0.5f);
            offset_array[2] = -static_cast<int>(offset * 2 / 6.0f + 0.5f);
            offset_array[3] = 0;
            offset_array[3] = static_cast<int>(offset * 2 / 6.0f + 0.5f);
            offset_array[4] = static_cast<int>(offset * 4 / 6.0f + 0.5f);
            offset_array[5] = offset;
            break;
        default:
            break;
        }

        for(int i = 0; i < p->children_num; i++){
            QPoint posNi = QPoint(pos.x() + offset_array[i], pos.y() + nodeHeight);
            painter.drawLine(getMidBottomPos(pos), getMidUpPos(posNi));
            QRect nodeRect = QRect(getLeftUpPos(posNi).x(), getLeftUpPos(posNi).y(), columnSize, rowSize);
            painter.drawEllipse(nodeRect);
            painter.drawText(nodeRect, p->children[i]->name, QTextOption(Qt::AlignCenter));

            p->children[i]->pos = posNi;
            queue.enqueue(p->children[i]);
        }
    }
    QLabel::paintEvent(event);
}

//*****************************test****************************test***************************test**********************************//

test::test(QWidget *parent) : QWidget(parent)
{
    initUi();
}

void test::initUi()
{
    resize(800, 600);
    //setFixedSize(800, 600);

    Tree::SyntaxTree * tree = Tree::newSyntaxTree();
    tree->root = Tree::newSyntaxTreeNode("root");
    Tree::SyntaxTreeNode * newNode;
    Tree::SyntaxTreeNode * aNode;
    Tree::SyntaxTreeNode * bNode;
    Tree::SyntaxTreeNode * cNode;
//    newNode = Tree::newSyntaxTreeNode("a");
//    aNode = newNode;
//    Tree::SyntaxTreeNode_AddChild(tree->root, newNode);
//    newNode = Tree::newSyntaxTreeNode("b");
//    bNode = newNode;
//    Tree::SyntaxTreeNode_AddChild(tree->root, newNode);
//    newNode = Tree::newSyntaxTreeNode("c");
//    cNode = newNode;
//    Tree::SyntaxTreeNode_AddChild(tree->root, newNode);
//    newNode = Tree::newSyntaxTreeNode("d");
//    Tree::SyntaxTreeNode_AddChild(tree->root, newNode);

//    Tree::SyntaxTreeNode * a1Node;
//    newNode = Tree::newSyntaxTreeNode("a1");
//    a1Node = newNode;
//    Tree::SyntaxTreeNode_AddChild(aNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("a2");
//    Tree::SyntaxTreeNode_AddChild(aNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("e1");
//    Tree::SyntaxTreeNode_AddChild(a1Node, newNode);
//    newNode = Tree::newSyntaxTreeNode("e2");
//    Tree::SyntaxTreeNode_AddChild(a1Node, newNode);

//    newNode = Tree::newSyntaxTreeNode("b1");
//    Tree::SyntaxTreeNode_AddChild(bNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("b2");
//    Tree::SyntaxTreeNode_AddChild(bNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("b3");
//    Tree::SyntaxTreeNode_AddChild(bNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("b4");
//    Tree::SyntaxTreeNode_AddChild(bNode, newNode);

//    newNode = Tree::newSyntaxTreeNode("c1");
//    Tree::SyntaxTreeNode_AddChild(cNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("c2");
//    Tree::SyntaxTreeNode_AddChild(cNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("c3");
//    Tree::SyntaxTreeNode_AddChild(cNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("c4");
//    Tree::SyntaxTreeNode_AddChild(cNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("c5");
//    Tree::SyntaxTreeNode_AddChild(cNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("c6");
//    Tree::SyntaxTreeNode_AddChild(cNode, newNode);
//    newNode = Tree::newSyntaxTreeNode("c7");
//    Tree::SyntaxTreeNode_AddChild(cNode, newNode);

    newNode = Tree::newSyntaxTreeNode("a");
    aNode = newNode;
    Tree::SyntaxTreeNode_AddChild(tree->root, newNode);
    newNode = Tree::newSyntaxTreeNode("b");
    bNode = newNode;
    Tree::SyntaxTreeNode_AddChild(tree->root, newNode);
    newNode = Tree::newSyntaxTreeNode("c");
    cNode = newNode;
    Tree::SyntaxTreeNode_AddChild(tree->root, newNode);

    Tree::SyntaxTreeNode * a1Node;
    newNode = Tree::newSyntaxTreeNode("a1");
    a1Node = newNode;
    Tree::SyntaxTreeNode_AddChild(aNode, newNode);
    newNode = Tree::newSyntaxTreeNode("a2");
    Tree::SyntaxTreeNode_AddChild(a1Node, newNode);

    newNode = Tree::newSyntaxTreeNode("c1");
    Tree::SyntaxTreeNode *c1Node = newNode;
    Tree::SyntaxTreeNode_AddChild(cNode, newNode);
    newNode = Tree::newSyntaxTreeNode("c2");
    Tree::SyntaxTreeNode *c2Node = newNode;
    Tree::SyntaxTreeNode_AddChild(c1Node, newNode);
    newNode = Tree::newSyntaxTreeNode("c3");
    Tree::SyntaxTreeNode_AddChild(c1Node, newNode);
    newNode = Tree::newSyntaxTreeNode("c4");
    Tree::SyntaxTreeNode *c4Node = newNode;
    Tree::SyntaxTreeNode_AddChild(c1Node, newNode);

    newNode = Tree::newSyntaxTreeNode("c5");
    Tree::SyntaxTreeNode_AddChild(c2Node, newNode);

    newNode = Tree::newSyntaxTreeNode("c6");
    Tree::SyntaxTreeNode *c6Node = newNode;
    Tree::SyntaxTreeNode_AddChild(c4Node, newNode);

    newNode = Tree::newSyntaxTreeNode("c7");
    Tree::SyntaxTreeNode_AddChild(c6Node, newNode);

    MyLabel *lbl = new MyLabel(this, tree);
    lbl->setFixedSize(5000, 1030);

    QWidget *mainWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(mainWidget);
    layout->setSpacing(0);
    layout->addWidget(lbl);
    layout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(mainWidget);

    QHBoxLayout *fatherLayout = new QHBoxLayout(this);
    fatherLayout->setSpacing(0);
    fatherLayout->addWidget(scrollArea);
    fatherLayout->setContentsMargins(0, 0, 0, 0);
}
