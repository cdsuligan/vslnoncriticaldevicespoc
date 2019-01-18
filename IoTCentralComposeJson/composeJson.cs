using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using System.Web.Http;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.Azure.WebJobs.Host;
using Newtonsoft.Json;
namespace FunctionApp1

{

    public static class IoTCentralComposeJson{

        [FunctionName("composeJson")]

        public static async Task<HttpResponseMessage> Run([HttpTrigger(AuthorizationLevel.Anonymous, "get", "post", Route = null)]HttpRequestMessage req, TraceWriter log)
        {

            var configuration = new HttpConfiguration();
            req.SetConfiguration(configuration);
            log.Info("C# HTTP trigger function processed a request.");
            string data = await req.Content.ReadAsStringAsync();
            var e = JsonConvert.DeserializeObject<DataInfo>(data);

            OutputJson outputJsons = new OutputJson();
            Content contents = new Content();
            Properties properties = new Properties();
            MachineEvent me = new MachineEvent();

            me.AC = e.Content.AC;
            me.D0Status = e.Content.D0Status;
            /*me.D0Status = e.Content.D1Status;
            me.D0Status = e.Content.D2Status;
            me.D0Status = e.Content.D3Status;
            me.D0Status = e.Content.D4Status;
            me.D0Status = e.Content.D5Status;
            me.D0Status = e.Content.D6Status;*/
            me.A0Status = e.Content.A0Status;
            me.IothubConnectionDeviceId = e.Properties.IothubConnectionDeviceId;
            me.IothubMessageSource = e.Properties.IothubMessageSource;

            outputJsons.Id = Guid.NewGuid().ToString();
            outputJsons.critOrNon = "Non-Critical";
            outputJsons.Publisher = "IoTCentral";
            outputJsons.PublisherId = "IoTC";
            outputJsons.PublisherVersion = "1.0.0.1";
            outputJsons.Timestamp = DateTime.UtcNow.ToString();
            outputJsons.MachineEvent = me;

            //Debug.WriteLine(e.ToString());

            var json = JsonConvert.SerializeObject(outputJsons);

            return req.CreateResponse(HttpStatusCode.OK, json, "application/json");
            //return (ActionResult)new OkObjectResult(json);

        }

    }
    public class MachineEvent
    {

        public decimal? AC { get; set; }
        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
        public string D0Status { get; set; }

        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
        public string A0Status { get; set; }
        [JsonProperty("DeviceId")]
        public string IothubConnectionDeviceId { get; set; }
        [JsonProperty("MessageSource")]
        public string IothubMessageSource { get; set; }
    }

    public class OutputJson
    {
        public string Id { get; set; }
        [JsonProperty("Type")]
        public string critOrNon { get; set; }
        public string Publisher { get; set; }
        public string PublisherId { get; set; }
        public string PublisherVersion { get; set; }
        public string Timestamp { get; set; }
        public MachineEvent MachineEvent { get; set; }
    }


    public class DataInfo
    {
        public Content Content { get; set; }
        public Properties Properties { get; set; }
    }

    public class Content
    {
        public decimal? AC { get; set; }
        [JsonProperty("D0Status")]
        public string D0Status { get; set; }
        [JsonProperty("A0Status")]
        public string A0Status { get; set; }
    }

    public class Properties
    {
        [JsonProperty("iothub-connection-device-id")]
        public string IothubConnectionDeviceId { get; set; }
        [JsonProperty("iothub-connection-auth-method")]
        public string IothubConnectionAuthMethod { get; set; }
        [JsonProperty("iothub-connection-auth-generation-id")]
        public string IothubConnectionAuthGenerationId { get; set; }
        [JsonProperty("iothub-enqueuedtime")]
        public string IothubEnqueuedtime { get; set; }
        [JsonProperty("iothub-message-source")]
        public string IothubMessageSource { get; set; }
        [JsonProperty("x-opt-sequence-number")]
        public int XOptSequenceNumber { get; set; }
        [JsonProperty("x-opt-offset")]
        public string XOptOffset { get; set; }
        [JsonProperty("x-opt-enqueued-time")]
        public string XOptEnqueuedTime { get; set; }
    }

}