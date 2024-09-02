import matplotlib.pyplot as plt
from statsmodels.tsa.arima.model import ARIMA

# 시간대별 이벤트 발생 빈도를 시각화
hourly_activity.plot(kind='bar', figsize=(10, 6))
plt.title('Hourly Activity')
plt.xlabel('Hour of the Day')
plt.ylabel('Number of Events')
plt.show()

# 시계열 모델을 이용한 패턴 예측 (ARIMA 모델)
model = ARIMA(hourly_activity, order=(1, 1, 1))
model_fit = model.fit()

# 예측 결과
forecast = model_fit.forecast(steps=24)
print(forecast)

# 예측 결과 시각화
plt.figure(figsize=(10, 6))
plt.plot(hourly_activity.index, hourly_activity, label='Observed')
plt.plot(forecast.index, forecast, label='Forecast', color='red')
plt.title('Hourly Activity Forecast')
plt.xlabel('Hour of the Day')
plt.ylabel('Number of Events')
plt.legend()
plt.show()
