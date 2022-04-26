#include <QApplication>
#include <QPushButton>
#include <QPainter>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QPixmap>
#include <fstream>
#include <iostream>
#include <vector>

#include "RenderArea.h"
#include "PointProcessor.h"
#include "Trie.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QFile statesFile;
  QJsonParseError jsonError;
  statesFile.setFileName("states.json");
  statesFile.open(QIODevice::ReadOnly | QIODevice::Text);
  QJsonObject obj = QJsonDocument::fromJson(statesFile.readAll(), &jsonError).object();
  statesFile.close();

  QVector<QString> states;
  QVector<QPair<QPolygon, QColor>> polygons;
  std::vector<double> statesSumOfToxicity;
  std::vector<double> statesCountOfTweets;

  // ========== BUILD POLYGONS ============ //
  int color_id = 0;
  foreach (QString key, obj.keys()) {
    states.push_back(key);
    statesSumOfToxicity.push_back(0);
    statesCountOfTweets.push_back(0);
    QJsonArray state_arr = obj.value(key).toArray();
    if (state_arr.size() == 1) {
      QPolygon polygon;
      foreach (const QJsonValue &coord, state_arr[0].toArray()) {
        QJsonArray parsed_point = coord.toArray();
        int x, y;
        PointProcessor::get_point(x, y, parsed_point[0].toDouble(), parsed_point[1].toDouble());
        polygon << QPoint(x, y);
      }
      polygons.push_back({polygon, QColor(color_id, color_id, 69)});
      // array of coords
    } else {
      // array of array of coords
      foreach (const QJsonValue &poly, state_arr) {
        QPolygon polygon;
        foreach (const QJsonValue &coord, poly.toArray()[0].toArray()) {
          QJsonArray parsed_point = coord.toArray();
          int x, y;
          PointProcessor::get_point(x, y, parsed_point[0].toDouble(), parsed_point[1].toDouble());
          polygon << QPoint(x, y);
        }
        polygons.push_back({polygon, QColor(color_id, color_id, 69)});
      }
    }
    color_id += 4;
  }

  RenderArea area(polygons); // my widget that I render
//
//  area.show();
//  return QApplication::exec();

  QPixmap pixmap = area.grab();
  QImage image(pixmap.toImage());
  std::ifstream in("../sentiments.csv");
  std::ios::sync_with_stdio(false);
  in.tie(0);
  Trie tr;
  std::string s;
  s.reserve(280);
  bool eof = false;
  while (!eof) {
    double x;
    getline(in, s);
    if (s.empty()) {
      eof = true;
      break;
    }
    std::string s_rank;
    while (*s.rbegin() != ',') {
      s_rank += *s.rbegin();
      s.pop_back();
    }
    s.pop_back();
    reverse(s_rank.begin(), s_rank.end());
    x = atof(s_rank.c_str());
    tr.add(s, x);
  }
  in.close();
  qDebug() << tr.sz();

  clock_t start, end;
  start = clock();
  FILE *file = fopen("../tweets_clear.txt", "r");
  char str[355];
  // 1568630
  int st = 0;
  for (int w = 0; w < 1568630; w++) {
//    float d; int x;
//    char cc;
//    fscanf(file, "[%f, %f]\t_\t%d-%d-%d %d:%d:%d\t", &d, &d, &x, &x, &x, &x, &x, &x);
    fgets(str, 350, file);
    int i = 1;
    int j = 0;
    char coord_y_s[30], coord_x_s[30];
    while (str[i] != ','){
      coord_y_s[j] = str[i];
      i++; j++;
    }
    coord_y_s[j] = '\0';
    i += 2;
    j = 0;
    while (str[i] != ']'){
      coord_x_s[j] = str[i];
      i++; j++;
    }
    coord_x_s[j] = '\0';
    double coord_y, coord_x;
    coord_y = atof(coord_y_s);
    coord_x = atof(coord_x_s);
    char tweet[281];
    i += 24;
    j = 0;
    while (str[i] != '\0'){
      tweet[j] = str[i];
      i++; j++;
    }
    tweet[j] = '\0';
    int x, y;
    PointProcessor::get_point(x, y, coord_x, coord_y);
    if (x <= 1300 && y <= 700){
      QColor color = image.pixelColor(x, y);
      if (color.blue() == 69){
        double rank = tr.get_rank(tweet);
        statesSumOfToxicity[color.red() / 4] += rank;
        statesCountOfTweets[color.red() / 4]++;
      }
    }
//    cout << tr.get_rank(str) << endl;
//    printf("%d %d %d %s\n", w + 1, x, y, tweet);
  }
  end = clock();
  double mn = 10, mx = -10;
  for (int i = 0; i < states.size(); i++){
    qDebug() << states[i] << " " << statesSumOfToxicity[i] / statesCountOfTweets[i];
    if (statesSumOfToxicity[i] / statesCountOfTweets[i] < mn){
      mn = statesSumOfToxicity[i] / statesCountOfTweets[i];
    }
    if (statesSumOfToxicity[i] / statesCountOfTweets[i] > mx){
      mx = statesSumOfToxicity[i] / statesCountOfTweets[i];
    }
  }
  qDebug() << mn << " " << mx;
  double step = (mx - mn) * 10000 / 255;
  std::vector<int> negativity;
  for (int i = 0; i < states.size(); i++){
    if (statesCountOfTweets[i] != 0){
      negativity.push_back((int)(((statesSumOfToxicity[i] * 10000 / statesCountOfTweets[i]) - (10000 * mn)) / step));
    } else {
      negativity.push_back(128); // neutral
    }
    qDebug() << negativity[i];
  }

  for (int i = 0; i < polygons.size(); i++){
    int state = polygons[i].second.red() / 4;
    polygons[i].second = QColor(negativity[state], 0, 255 - negativity[state]);
  }
  RenderArea goodMap(polygons);
  goodMap.show();

  return QApplication::exec();
}
